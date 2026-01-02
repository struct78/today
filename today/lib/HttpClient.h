// HttpClient.h
#pragma once
#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include "Logger.h"

struct HttpResponse {
  int statusCode;
  char* body;          // Changed to char pointer for large responses
  int bodyLength;      // Track the actual length
  int bodyCapacity;    // Track allocated capacity
  bool isSuccess;
  String error;

  HttpResponse() : body(nullptr), bodyLength(0), bodyCapacity(0) {}

  ~HttpResponse() {
    if (body) {
      free(body);
      body = nullptr;
    }
  }

  // Copy constructor
  HttpResponse(const HttpResponse& other) {
    statusCode = other.statusCode;
    isSuccess = other.isSuccess;
    error = other.error;
    bodyLength = other.bodyLength;
    bodyCapacity = other.bodyCapacity;

    if (other.body && other.bodyLength > 0) {
      body = (char*)malloc(bodyCapacity);
      if (body) {
        memcpy(body, other.body, bodyLength);
      }
    }
    else {
      body = nullptr;
    }
  }

  // Assignment operator
  HttpResponse& operator=(const HttpResponse& other) {
    if (this != &other) {
      if (body) {
        free(body);
      }

      statusCode = other.statusCode;
      isSuccess = other.isSuccess;
      error = other.error;
      bodyLength = other.bodyLength;
      bodyCapacity = other.bodyCapacity;

      if (other.body && other.bodyLength > 0) {
        body = (char*)malloc(bodyCapacity);
        if (body) {
          memcpy(body, other.body, bodyLength);
        }
      }
      else {
        body = nullptr;
      }
    }
    return *this;
  }
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
    HttpResponse response;
    response.statusCode = 0;
    response.isSuccess = false;

    if (!checkConnection()) {
      response.error = "WiFi not connected";
      Logger::log(response.error);
      return response;
    }

    Logger::log("HTTP GET: https://" + host + path);

    // // For problematic APIs, use manual HTTP instead of ArduinoHttpClient
    // if (host == "api.canwegointhepool.com") {
    //   return getManualHttp(host, path, port);
    // }

    // For other APIs, use the standard ArduinoHttpClient
    HttpClient http = HttpClient(wifiSSLClient, host, port);

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
    HttpResponse response;
    response.statusCode = 0;
    response.isSuccess = false;

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

    // Check content length if available
    int contentLength = http.contentLength();
    Logger::log("Content length: " + String(contentLength));

    // Try to read body with better handling for large responses
    Logger::log("Reading response body...");

    // Allocate buffer for large response (100KB should be enough)
    int bufferSize = (contentLength > 0) ? contentLength + 100 : 100000;
    response.body = (char*)malloc(bufferSize);
    response.bodyCapacity = bufferSize;

    if (!response.body) {
      response.error = "Failed to allocate memory for response body";
      Logger::log(response.error);
      http.stop();
      return response;
    }

    Logger::log("Allocated " + String(bufferSize) + " bytes for response body");

    unsigned long bodyStart = millis();
    int totalBytesRead = 0;

    // Read character by character into char array
    while (millis() - bodyStart < 60000 && totalBytesRead < bufferSize - 1) {
      if (http.available()) {
        char c = http.read();
        response.body[totalBytesRead] = c;
        totalBytesRead++;

        // Log progress every 10KB
        if (totalBytesRead % 10000 == 0) {
          Logger::log("Read " + String(totalBytesRead) + " bytes so far");
        }
      }
      else {
        delay(10); // Small delay when no data available
        
        // If we haven't received any data for 5 seconds and the connection is closed, break
        if (millis() - bodyStart > 5000 && !http.connected()) {
          Logger::log("Connection closed and no more data expected");
          break;
        }
      }

      // If we know the content length and have read it all, break
      if (contentLength > 0 && totalBytesRead >= contentLength) {
        Logger::log("Read all expected content (" + String(contentLength) + " bytes)");
        break;
      }
    }

    // Null terminate the response
    response.body[totalBytesRead] = '\0';
    response.bodyLength = totalBytesRead;
    Logger::log("Response received, length: " + String(response.bodyLength));

    if (response.statusCode == 200) {
      response.isSuccess = true;
      Logger::log("HTTP request successful (200 OK)");
    }
    else {
      response.error = "HTTP error: " + String(response.statusCode);
      Logger::log(response.error);
      if (response.bodyLength > 0 && response.bodyLength < 500) {
        Logger::log("Response body: " + String(response.body));
      }
    }

    http.stop();
    return response;
  }
};