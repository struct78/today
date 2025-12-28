// WeatherForecast.h
#pragma once
#include <ArduinoJson.h>
#include <WiFi.h>
#include "HttpClient.h"

struct HourlyForecastData {
  String time;
  float temperature;
  float rainAccumulation;
  bool hasRain;
  bool isValid;
};

struct DailyForecastData {
  String date;
  float cloudCoverAvg;
  float temperatureApparentAvg;
  float temperatureAvg;
  float temperatureMin;
  float temperatureMax;
  float uvIndexAvg;
  float windSpeedAvg;
  float windDirectionAvg;
  float rainAccumulationAvg;
  bool hasRain;
  bool isValid;
};

struct ForecastData {
  static const int MAX_DAYS = 7;
  static const int MAX_HOURS = 9;
  DailyForecastData daily[MAX_DAYS];
  HourlyForecastData hourly[MAX_HOURS];
  int dayCount;
  int hourCount;
  bool isValid;
};

class WeatherForecast {
private:
  String apiKey;
  String location;
  SimpleHttpClient httpClient;

public:
  WeatherForecast(const String& key, const String& loc)
    : apiKey(key), location(loc) {
  }

  ForecastData fetchForecastData() {
    ForecastData data = { {}, {}, 0, 0, false };

    String queryParams = "location=" + location + "&apikey=" + apiKey + "&timesteps=1h,1d";
    HttpResponse response = httpClient.get("api.tomorrow.io", "/v4/weather/forecast", queryParams);

    if (!response.isSuccess) {
      Serial.println("Failed to fetch forecast data: " + response.error);
      return data;
    }

    Serial.println(response.body);

    if (parseForecastJson(response.body, data)) {
      data.isValid = true;
    }

    return data;
  }

  bool parseForecastJson(const String& jsonString, ForecastData& data) {
    DynamicJsonDocument doc(16384); // Larger buffer for both hourly and daily data

    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
      Serial.println("JSON parsing failed");
      return false;
    }

    // Parse hourly data
    data.hourCount = 0;
    if (doc["timelines"]["hourly"]) {
      JsonArray hourlyArray = doc["timelines"]["hourly"];
      data.hourCount = min((int)hourlyArray.size(), ForecastData::MAX_HOURS);

      for (int i = 0; i < data.hourCount; i++) {
        JsonObject hour = hourlyArray[i];
        JsonObject values = hour["values"];

        data.hourly[i].time = hour["time"].as<String>();
        data.hourly[i].temperature = values["temperature"] | 0.0f;
        data.hourly[i].rainAccumulation = values["rainAccumulation"] | 0.0f;
        data.hourly[i].hasRain = data.hourly[i].rainAccumulation > 0;
        data.hourly[i].isValid = true;
      }
    }

    // Parse daily data
    data.dayCount = 0;
    if (doc["timelines"]["daily"]) {
      JsonArray dailyArray = doc["timelines"]["daily"];
      data.dayCount = min((int)dailyArray.size(), ForecastData::MAX_DAYS);

      for (int i = 0; i < data.dayCount; i++) {
        JsonObject day = dailyArray[i];
        JsonObject values = day["values"];

        data.daily[i].date = day["time"].as<String>();
        data.daily[i].cloudCoverAvg = values["cloudCoverAvg"] | 0.0f;
        data.daily[i].temperatureApparentAvg = values["temperatureApparentAvg"] | 0.0f;
        data.daily[i].temperatureAvg = values["temperatureAvg"] | 0.0f;
        data.daily[i].temperatureMin = values["temperatureMin"] | 0.0f;
        data.daily[i].temperatureMax = values["temperatureMax"] | 0.0f;
        data.daily[i].uvIndexAvg = values["uvIndexAvg"] | 0.0f;
        data.daily[i].windSpeedAvg = values["windSpeedAvg"] | 0.0f;
        data.daily[i].windDirectionAvg = values["windDirectionAvg"] | 0.0f;
        data.daily[i].rainAccumulationAvg = values["rainAccumulationAvg"] | 0.0f;
        data.daily[i].hasRain = data.daily[i].rainAccumulationAvg > 0;
        data.daily[i].isValid = true;
      }
    }

    return data.dayCount > 0 || data.hourCount > 0;
  }
};
