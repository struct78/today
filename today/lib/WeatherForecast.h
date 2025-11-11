// WeatherForecast.h
#pragma once
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>

struct DailyForecastData
{
  String date;
  float cloudCoverAvg;
  float temperatureApparentAvg;
  float temperatureAvg;
  float uvIndexAvg;
  float windSpeedAvg;
  float windDirectionAvg;
  bool isValid;
};

struct ForecastData
{
  static const int MAX_DAYS = 7;
  DailyForecastData daily[MAX_DAYS];
  int dayCount;
  bool isValid;
};

class WeatherForecast
{
private:
  String apiKey;
  String location;
  WiFiSSLClient wifiSSLClient;

public:
  WeatherForecast(const String &key, const String &loc)
      : apiKey(key), location(loc) {}

  ForecastData fetchForecastData()
  {
    ForecastData data = {{}, 0, false};

    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi not connected");
      return data;
    }

    // Create HTTPS client for Arduino Giga
    HttpClient http = HttpClient(wifiSSLClient, "api.tomorrow.io", 443);

    String path = "/v4/weather/forecast?location=" + location + "&apikey=" + apiKey + "&timesteps=1d";

    http.beginRequest();
    http.get(path);
    http.sendHeader("accept", "application/json");
    http.endRequest();

    int statusCode = http.responseStatusCode();

    if (statusCode == 200)
    {
      String payload = http.responseBody();

      Serial.println(payload);

      if (parseForecastJson(payload, data))
      {
        data.isValid = true;
      }
    }
    else
    {
      Serial.print("HTTP error: ");
      Serial.println(statusCode);
    }

    http.stop();
    return data;
  }

  bool parseForecastJson(const String &jsonString, ForecastData &data)
  {
    DynamicJsonDocument doc(8192); // Larger buffer for forecast data

    DeserializationError error = deserializeJson(doc, jsonString);
    if (error)
    {
      Serial.println("JSON parsing failed");
      return false;
    }

    if (!doc["timelines"]["daily"])
    {
      Serial.println("No daily timeline found");
      return false;
    }

    JsonArray dailyArray = doc["timelines"]["daily"];
    data.dayCount = min((int)dailyArray.size(), ForecastData::MAX_DAYS);

    for (int i = 0; i < data.dayCount; i++)
    {
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
