// WeatherRealtime.h
#pragma once
#include <ArduinoJson.h>
#include <WiFi.h>
#include "Logger.h"
#include "HttpClient.h"

struct RealtimeWeatherData {
  float temperature;
  float uvIndex;
  float humidity;
  float windSpeed;
  float windDirection;
  float cloudCover;
  bool isValid;
};

class WeatherRealtime {
private:
  String apiKey;
  String location;
  SimpleHttpClient httpClient;

public:
  WeatherRealtime(const String& key, const String& loc)
    : apiKey(key), location(loc) {
  }

  RealtimeWeatherData fetchWeatherData() {
    RealtimeWeatherData data = { 0, 0, 0, 0, 0, 0, false };

    String queryParams = "location=" + location + "&apikey=" + apiKey;
    HttpResponse response = httpClient.get("api.tomorrow.io", "/v4/weather/realtime", queryParams);

    if (!response.isSuccess) {
      Logger::log("Failed to fetch weather data: " + response.error);
      return data;
    }

    if (!parseRealtimeJson(response.body, data)) {
      Logger::log("Failed to parse weather data");
      return data;
    }

    data.isValid = true;
    return data;
  }

  bool parseRealtimeJson(const String& jsonString, RealtimeWeatherData& data) {
    DynamicJsonDocument doc(2048);

    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
      return false;
    }

    if (doc["data"]["values"]) {
      JsonObject values = doc["data"]["values"];
      data.temperature = values["temperature"] | 0.0f;
      data.uvIndex = values["uvIndex"] | 0.0f;
      data.humidity = values["humidity"] | 0.0f;
      data.windSpeed = values["windSpeed"] | 0.0f;
      data.windDirection = values["windDirection"] | 0.0f;
      data.cloudCover = values["cloudCover"] | 0.0f;
      data.isValid = true;
      return true;
    }

    return false;
  }
};
