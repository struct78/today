// HttpClient.h
#pragma once
#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include "Logger.h"

struct HttpResponse {
  int statusCode;
  String body;
  bool isSuccess;
  String error;
};

class SimpleHttpClient {
private:
  WiFiSSLClient wifiSSLClient;

public:
  SimpleHttpClient() {
  }

  HttpResponse get(const String& host, const String& path, int port = 443) {
    HttpResponse response = { 0, "", false, "" };

    if (!checkConnection()) {
      response.error = "WiFi not connected";
      Logger::log(response.error);
      return response;
    }

    // Test SSL connection first
    if (!testConnection(host, port)) {
      response.error = "Cannot establish SSL connection to " + host;
      Logger::log(response.error);
      return response;
    }

    // Create HTTPS client with timeout
    HttpClient http = HttpClient(wifiSSLClient, host, port);
    http.connectionKeepAlive(); // Keep connection alive for efficiency
    
    Logger::log("HTTP GET: https://" + host + path);

    // Set timeout for connection and response
    wifiSSLClient.setTimeout(10000); // 10 second timeout

    if (!beginRequest(http, path, response)) {
      return response;
    }

    return processResponse(http);
  }

  HttpResponse get(const String& host, const String& path, const String& queryParams, int port = 443) {
    String fullPath = path;
    if (queryParams.length() > 0) {
      fullPath += "?" + queryParams;
    }
    return get(host, fullPath, port);
  }

private:
  bool checkConnection() {
    return WiFi.status() == WL_CONNECTED;
  }

  bool testConnection(const String& host, int port) {
    Logger::log("Testing SSL connection to " + host + ":" + String(port));
    
    if (wifiSSLClient.connect(host.c_str(), port)) {
      Logger::log("SSL connection test successful");
      wifiSSLClient.stop();
      return true;
    } else {
      Logger::log("SSL connection test failed");
      return false;
    }
  }

  bool beginRequest(HttpClient& http, const String& path, HttpResponse& response) {
    Logger::log("Starting HTTP request...");
    
    int result = http.beginRequest();
    if (result != 0) {
      response.error = "Failed to begin request: " + String(result);
      Logger::log(response.error);
      return false;
    }
    
    result = http.get(path);
    if (result != 0) {
      response.error = "Failed to send GET request: " + String(result);
      Logger::log(response.error);
      return false;
    }
    
    http.sendHeader("accept", "application/json");
    http.sendHeader("user-agent", "Arduino/1.0");
    
    result = http.endRequest();
    if (result != 0) {
      response.error = "Failed to end request: " + String(result);
      Logger::log(response.error);
      return false;
    }
    
    Logger::log("HTTP request sent successfully");
    return true;
  }

  HttpResponse processResponse(HttpClient& http) {
    HttpResponse response = { 0, "", false, "" };

    Logger::log("Waiting for response...");
    
    // Wait for response with timeout
    unsigned long start = millis();
    while (!http.available() && millis() - start < 10000) {
      delay(100);
    }
    
    if (!http.available()) {
      response.error = "Request timeout - no response received";
      Logger::log(response.error);
      http.stop();
      return response;
    }

    response.statusCode = http.responseStatusCode();
    Logger::log("Response status: " + String(response.statusCode));
    
    if (response.statusCode <= 0) {
      response.error = "Invalid response status: " + String(response.statusCode);
      Logger::log(response.error);
      http.stop();
      return response;
    }
    
    response.body = http.responseBody();
    Logger::log("Response received, length: " + String(response.body.length()));

    if (response.statusCode == 200) {
      response.isSuccess = true;
      Logger::log("HTTP request successful (200 OK)");
    }
    else {
      response.error = "HTTP error: " + String(response.statusCode);
      Logger::log(response.error);
      if (response.body.length() > 0) {
        Logger::log("Response body: " + response.body);
      }
    }

    http.stop();
    return response;
  }
};