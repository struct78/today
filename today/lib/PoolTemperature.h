// PoolTemperature.h
#pragma once
#include <ArduinoJson.h>
#include <WiFi.h>
#include "Logger.h"
#include "HttpClient.h"
#include "TimeManager.h"

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
  TimeManager* timeManager;

public:
  PoolTemperature(TimeManager* timeManager) : timeManager(timeManager) {
  }

  PoolTemperatureData fetchPoolData() {
    Logger::log("=== Starting pool temperature fetch ===");
    PoolTemperatureData data = { "", 0.0f, 0, "", false };

    Logger::log("Making HTTP request to pool API...");
    HttpResponse response = httpClient.get("api.canwegointhepool.com", "/app/read");
    Logger::log("HTTP request completed");

    if (!response.isSuccess) {
      Logger::log("Failed to fetch pool data: " + response.error);
      return data;
    }

    Logger::log("Pool API response: " + response.body);

    if (parsePoolJson(response.body, data)) {
      data.timeAgo = timeManager->formatTimeAgo(data.timestamp);
      data.isValid = true;
      Logger::log("Pool data parsed successfully");
    } else {
      Logger::log("Failed to parse pool data");
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

public:

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