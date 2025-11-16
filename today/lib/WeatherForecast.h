// WeatherForecast.h
#pragma once
#include <ArduinoJson.h>
#include <WiFi.h>
#include "HttpClient.h"

struct DailyForecastData {
  String date;
  float cloudCoverAvg;
  float temperatureApparentAvg;
  float temperatureAvg;
  float uvIndexAvg;
  float windSpeedAvg;
  float windDirectionAvg;
  bool isValid;
};

struct ForecastData {
  static const int MAX_DAYS = 7;
  DailyForecastData daily[MAX_DAYS];
  int dayCount;
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
    ForecastData data = { {}, 0, false };

    String queryParams = "location=" + location + "&apikey=" + apiKey + "&timesteps=1d";
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
    DynamicJsonDocument doc(8192); // Larger buffer for forecast data

    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
      Serial.println("JSON parsing failed");
      return false;
    }

    if (!doc["timelines"]["daily"]) {
      Serial.println("No daily timeline found");
      return false;
    }

    JsonArray dailyArray = doc["timelines"]["daily"];
    data.dayCount = min((int)dailyArray.size(), ForecastData::MAX_DAYS);

    for (int i = 0; i < data.dayCount; i++) {
      JsonObject day = dailyArray[i];
      JsonObject values = day["values"];

      data.daily[i].date = day["time"].as<String>();
      data.daily[i].cloudCoverAvg = values["cloudCoverAvg"] | 0.0f;
      data.daily[i].temperatureApparentAvg = values["temperatureApparentAvg"] | 0.0f;
      data.daily[i].temperatureAvg = values["temperatureAvg"] | 0.0f;
      data.daily[i].uvIndexAvg = values["uvIndexAvg"] | 0.0f;
      data.daily[i].windSpeedAvg = values["windSpeedAvg"] | 0.0f;
      data.daily[i].windDirectionAvg = values["windDirectionAvg"] | 0.0f;
      data.daily[i].isValid = true;
    }

    return data.dayCount > 0;
  }
};
