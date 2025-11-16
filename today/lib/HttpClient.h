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
    // Initialize SSL client
    wifiSSLClient.flush();
  }

  HttpResponse get(const String& host, const String& path, int port = 443, bool useSSLConnect = false) {
    HttpResponse response = { 0, "", false, "" };

    if (!checkConnection()) {
      response.error = "WiFi not connected";
      Logger::log(response.error);
      return response;
    }

    Logger::log("HTTP GET: https://" + host + path);

    // For problematic APIs, use manual HTTP instead of ArduinoHttpClient
    if (host == "api.canwegointhepool.com") {
      return getManualHttp(host, path, port);
    }

    // For other APIs, use the standard ArduinoHttpClient
    HttpClient http = HttpClient(wifiSSLClient, host, port);

    if (!beginRequest(http, path, response)) {
      return response;
    }

    return processResponse(http);
  }  HttpResponse get(const String& host, const String& path, const String& queryParams, int port = 443) {
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

  // Manual HTTP method for problematic APIs (inspired by working pool API code)
  HttpResponse getManualHttp(const String& host, const String& path, int port = 443) {
    HttpResponse response = { 0, "", false, "" };

    Logger::log("Using manual HTTP for: " + host);

    // Connect using SSL
    if (!wifiSSLClient.connectSSL(host.c_str(), port)) {
      response.error = "Failed to connect to " + host;
      Logger::log(response.error);
      return response;
    }

    Logger::log("SSL connection established");

    // Send manual HTTP request (similar to your working code)
    wifiSSLClient.print("GET ");
    wifiSSLClient.print(path.c_str());
    wifiSSLClient.println(" HTTP/1.1");
    wifiSSLClient.print("Host: ");
    wifiSSLClient.println(host.c_str());
    wifiSSLClient.println("User-Agent: Today/1.0");
    wifiSSLClient.println("Connection: close");
    wifiSSLClient.println();

    Logger::log("HTTP request sent manually");

    // Read response
    unsigned long startTime = millis();
    String responseText = "";
    bool headersEnded = false;
    int statusCode = 0;

    // Wait for response with timeout
    while (millis() - startTime < 30000) { // 30 second timeout
      if (wifiSSLClient.available()) {
        String line = "";

        // Read line character by character
        while (wifiSSLClient.available()) {
          char c = wifiSSLClient.read();
          if (c == '\n') break;
          if (c != '\r') line += c;
        }

        if (!headersEnded) {
          // Parse status line
          if (line.startsWith("HTTP/") && statusCode == 0) {
            int firstSpace = line.indexOf(' ');
            int secondSpace = line.indexOf(' ', firstSpace + 1);
            if (firstSpace > 0 && secondSpace > firstSpace) {
              statusCode = line.substring(firstSpace + 1, secondSpace).toInt();
              Logger::log("Status code: " + String(statusCode));
            }
          }

          // Check for end of headers
          if (line.length() <= 2) { // Empty line or just \r\n
            headersEnded = true;
            Logger::log("Headers ended, reading body...");
          }
        }
        else {
          // Read body
          responseText += line;
        }
      }
      else {
        delay(100);
      }
    }

    wifiSSLClient.stop();

    response.statusCode = statusCode;
    responseText.trim();
    response.body = responseText;

    if (statusCode == 200 && response.body.length() > 0) {
      response.isSuccess = true;
      Logger::log("Manual HTTP successful: " + String(response.body.length()) + " bytes");
    }
    else {
      response.error = "Manual HTTP failed or empty response";
      Logger::log(response.error);
    }

    return response;
  }

  bool testConnection(const String& host, int port, bool useSSLConnect = false) {
    Logger::log("Testing SSL connection to " + host + ":" + String(port));

    if (useSSLConnect && wifiSSLClient.connectSSL(host.c_str(), port)) {
      Logger::log("SSL connection (.connectSSL()) test successful");
      wifiSSLClient.stop();
      return true;
    }

    if (!useSSLConnect && wifiSSLClient.connect(host.c_str(), port)) {
      Logger::log("SSL connection (.connect()) test successful");
      wifiSSLClient.stop();
      return true;
    }

    Logger::log("SSL connection test failed");
    wifiSSLClient.stop();
    return false;
  }

  bool beginRequest(HttpClient& http, const String& path, HttpResponse& response) {
    Logger::log("Starting HTTP request...");

    // Add delay to ensure SSL connection is stable
    delay(100);

    http.beginRequest();
    http.get(path);
    http.sendHeader("accept", "application/json");
    http.sendHeader("user-agent", "Today 0.1");
    // Add some standard headers that might help with AWS API Gateway
    http.sendHeader("connection", "close");
    http.sendHeader("cache-control", "no-cache");
    http.endRequest();

    Logger::log("HTTP request sent successfully");

    // Add small delay after sending request
    delay(100);

    return true;
  }

  HttpResponse processResponse(HttpClient& http) {
    HttpResponse response = { 0, "", false, "" };

    Logger::log("Waiting for response...");

    // Wait for response with longer timeout for slower APIs
    unsigned long start = millis();
    int checkCount = 0;
    while (!http.available() && millis() - start < 30000) { // 30 second timeout
      delay(500); // Check every 500ms
      checkCount++;

      // Log progress every 5 seconds
      if (checkCount % 10 == 0) {
        Logger::log("Still waiting for response... (" + String((millis() - start) / 1000) + "s)");
      }
    }

    if (!http.available()) {
      response.error = "Request timeout - no response received after 30 seconds";
      Logger::log(response.error);
      Logger::log("Connection state: " + String(http.connected() ? "connected" : "disconnected"));
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