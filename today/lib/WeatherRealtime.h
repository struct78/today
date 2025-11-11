// WeatherRealtime.h
#pragma once
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>

struct RealtimeWeatherData {
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
  WiFiSSLClient wifiSSLClient;

public:
  WeatherRealtime(const String& key, const String& loc)
    : apiKey(key), location(loc) {
  }

  RealtimeWeatherData fetchWeatherData() {
    RealtimeWeatherData data = { 0, 0, 0, 0, 0, false };

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected");
      return data;
    }

    // Create HTTPS client for Arduino Giga
    HttpClient http = HttpClient(wifiSSLClient, "api.tomorrow.io", 443);

    String path = "/v4/weather/realtime?location=" + location + "&apikey=" + apiKey;

    Serial.println(path);

    http.beginRequest();
    http.get(path);
    http.sendHeader("accept", "application/json");
    http.endRequest();

    int statusCode = http.responseStatusCode();

    if (statusCode == 200) {
      String payload = http.responseBody();
      DynamicJsonDocument doc(2048);

      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.println("JSON parsing failed");
        http.stop();
        return data;
      }

      // Extract the required fields from data.values
      if (doc["data"]["values"]) {
        JsonObject values = doc["data"]["values"];
        data.uvIndex = values["uvIndex"] | 0.0f;
        data.humidity = values["humidity"] | 0.0f;
        data.windSpeed = values["windSpeed"] | 0.0f;
        data.windDirection = values["windDirection"] | 0.0f;
        data.cloudCover = values["cloudCover"] | 0.0f;
        data.isValid = true;
      }
    }
    else {
      String payload = http.responseBody();
      Serial.println(payload);
      Serial.print("HTTP error: ");
      Serial.println(statusCode);
    }

    http.stop();
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
