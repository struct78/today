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

    // Create HTTPS client
    HttpClient http = HttpClient(wifiSSLClient, host, port);

    Logger::log("HTTP GET: https://" + host + path);

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

  bool beginRequest(HttpClient& http, const String& path, HttpResponse& response) {
    http.beginRequest();
    http.get(path);
    http.sendHeader("accept", "application/json");
    http.endRequest();
    return true;
  }

  HttpResponse processResponse(HttpClient& http) {
    HttpResponse response = { 0, "", false, "" };

    response.statusCode = http.responseStatusCode();
    response.body = http.responseBody();

    if (response.statusCode == 200) {
      response.isSuccess = true;
      Logger::log("HTTP request successful (200 OK)");
    }
    else {
      response.error = "HTTP error: " + String(response.statusCode);
      Logger::log(response.error);
      Logger::log("Response body: " + response.body);
    }

    http.stop();
    return response;
  }
};