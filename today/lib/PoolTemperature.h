// PoolTemperature.h
#pragma once
#include <ArduinoJson.h>
#include <WiFi.h>
#include "Logger.h"
#include "HttpClient.h"

struct PoolTemperatureData {
  String id;
  float temperature;
  unsigned long timestamp;
  String timeAgo;
  bool isValid;
};

class PoolTemperature {
private:
  SimpleHttpClient httpClient;

public:
  PoolTemperature() {
  }

  PoolTemperatureData fetchPoolData() {
    PoolTemperatureData data = { "", 0.0f, 0, "", false };

    HttpResponse response = httpClient.get("api.canwegointhepool.com", "/app/read");

    if (!response.isSuccess) {
      Logger::log("Failed to fetch pool data: " + response.error);
      return data;
    }

    Logger::log("Pool API response: " + response.body);

    if (parsePoolJson(response.body, data)) {
      data.timeAgo = formatTimeAgo(data.timestamp);
      data.isValid = true;
    }

    return data;
  }

  bool parsePoolJson(const String& jsonString, PoolTemperatureData& data) {
    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, jsonString);
    if (error) {
      Logger::log("Pool JSON parsing failed: " + String(error.c_str()));
      return false;
    }

    if (doc["id"] && doc["temperature"] && doc["date"]) {
      data.id = doc["id"].as<String>();
      data.temperature = doc["temperature"] | 0.0f;
      data.timestamp = doc["date"] | 0UL;
      return true;
    }

    Logger::log("Pool JSON missing required fields");
    return false;
  }

  String formatTimeAgo(unsigned long timestamp) {
    if (timestamp == 0) {
      return "unknown";
    }

    // Get current time in seconds (timestamp appears to be in milliseconds)
    unsigned long currentTime = millis();

    // Convert timestamp from milliseconds to seconds for calculation
    unsigned long timestampSeconds = timestamp / 1000;
    unsigned long currentTimeSeconds = currentTime / 1000;

    // Calculate difference in seconds
    unsigned long diffSeconds;
    if (currentTimeSeconds > timestampSeconds) {
      diffSeconds = currentTimeSeconds - timestampSeconds;
    }
    else {
      // Handle case where timestamp is in the future or very recent
      diffSeconds = 0;
    }

    // Convert to more meaningful units
    if (diffSeconds < 60) {
      return String(diffSeconds) + " seconds ago";
    }
    else if (diffSeconds < 3600) { // Less than 1 hour
      unsigned long minutes = diffSeconds / 60;
      return String(minutes) + " minutes ago";
    }
    else if (diffSeconds < 86400) { // Less than 1 day
      unsigned long hours = diffSeconds / 3600;
      unsigned long minutes = (diffSeconds % 3600) / 60;

      if (minutes == 0) {
        return String(hours) + " hours ago";
      }
      else {
        return String(hours) + " hours " + String(minutes) + " minutes ago";
      }
    }
    else { // 1 day or more
      unsigned long days = diffSeconds / 86400;
      unsigned long hours = (diffSeconds % 86400) / 3600;

      if (hours == 0) {
        return String(days) + " days ago";
      }
      else {
        return String(days) + " days " + String(hours) + " hours ago";
      }
    }
  }

  // Load test data for offline mode
  PoolTemperatureData loadTestData() {
    const char* testPoolJson = R"({
      "id": "current",
      "temperature": 19.31,
      "date": 1731227559883
    })";

    PoolTemperatureData data = { "", 0.0f, 0, "", false };

    if (parsePoolJson(String(testPoolJson), data)) {
      data.timeAgo = "2 hours 13 minutes ago"; // Static test value
      data.isValid = true;
    }

    return data;
  }
};