#include <SPI.h>
#include <WiFi.h>
#include "credentials.h"
#include "lib/Logger.h"
#include "lib/TimeManager.h"
#include "lib/WeatherRealtime.h"
#include "lib/WeatherForecast.h"
#include "lib/PoolTemperature.h"
#include "lib/Display.h"

WeatherRealtime* realtimeWeather;
WeatherForecast* forecastWeather;
PoolTemperature* poolTemperature;
TimeManager* timeManager;

unsigned long lastUpdate = 0;
const unsigned long updateIntervalMs = 8 * 60 * 1000; // 8 minutes between updates (safe for 25 req/hour limit)
const bool offlineMode = false;
const int slideshowTimeMs = 4000;
bool isLoading = true;

// Function declarations
bool initializeWiFiConnection();
bool isUpdateRequired();
void clearScreen();
void fetchAndDisplayForecast();
void fetchAndDisplayRealtime();
void fetchAndDisplayPoolTemp();
void initializeOfflineMode();
void initializeSystem();
void initializeWeatherClients();
void updateWeatherData();
RealtimeWeatherData loadTestRealtimeData();
ForecastData loadTestForecastData();

void setup() {
  initializeSystem();

  delay(10000);

  if (offlineMode) {
    initializeOfflineMode();
    return;
  }

  if (!initializeWiFiConnection()) {
    delay(2000);
    return;
  }

  initializeWeatherClients();
}

void loop() {
  if (isLoading) {
    Logger::log("Loading phase - waiting 5 seconds...");
    delay(5000);
    isLoading = false;
    Logger::log("Loading complete - starting main loop");
  }

  // // Handle touch input for display toggle
  Display::handleTouchToggle();

  // // Update slideshow if weather data is available
  Display::updateSlideShow();

  // Sync with NTP periodically (every 6 hours) for accurate time
  if (!offlineMode && timeManager != nullptr) {
    timeManager->resyncIfNeeded();
  }

  // // Check if it's time to update weather data periodically
  if (isUpdateRequired()) {
    Serial.println("Updating weather data...");
    updateWeatherData();
    lastUpdate = millis();
  }

  delay(50);
}

void updateWeatherData() {
  if (!offlineMode && WiFi.status() != WL_CONNECTED) {
    Logger::log("WiFi not connected - skipping weather update");
    return;
  }

  // Don't clear screen here as slideshow will handle display
  fetchAndDisplayRealtime();
  fetchAndDisplayPoolTemp();
  // Remove forecast for now to focus on slideshow
  // fetchAndDisplayForecast();
}

void clearScreen() {
  Logger::log("Clearing screen...");
  Display::clearScreen();
  Logger::log("Screen cleared");
}

// Test data loading functions
RealtimeWeatherData loadTestRealtimeData() {
  // Test data from examples/realtime.json
  const char* testRealtimeJson = R"({
    "data": {
      "time": "2025-11-08T01:13:00Z",
      "values": {
        "altimeterSetting": 1010.93,
        "cloudBase": 0.4,
        "cloudCeiling": 0.5,
        "cloudCover": 96,
        "dewPoint": 10.7,
        "freezingRainIntensity": 0,
        "humidity": 92,
        "precipitationProbability": 0,
        "pressureSeaLevel": 1011.13,
        "pressureSurfaceLevel": 1011.42,
        "rainIntensity": 0,
        "sleetIntensity": 0,
        "snowIntensity": 0,
        "temperature": 11.8,
        "temperatureApparent": 11.8,
        "uvHealthConcern": 0,
        "uvIndex": 1,
        "visibility": 3.46,
        "weatherCode": 2100,
        "windDirection": 209,
        "windGust": 12.7,
        "windSpeed": 8.2
      }
    },
    "location": { "lat": -37.87644194695991, "lon": 145.06346130288253 }
  })";

  RealtimeWeatherData data = { 0, 0, 0, 0, 0, 0, false };

  // Use the existing parsing function from WeatherRealtime class
  if (!realtimeWeather || !realtimeWeather->parseRealtimeJson(String(testRealtimeJson), data)) {
    Logger::log("Failed to parse test realtime data");
    return data;
  }

  Logger::log("Test realtime data parsed successfully");
  data.isValid = true;  // Explicitly set isValid flag
  return data;
}

ForecastData loadTestForecastData() {
  Logger::log("=== Loading test forecast data ===");
  // Test data from examples/forecast.json (simplified for this example)
  const char* testForecastJson = R"({
    "timelines": {
      "daily": [
        {
          "time": "2025-11-08T00:00:00Z",
          "values": {
            "cloudCoverAvg": 61,
            "temperatureApparentAvg": 10.7,
            "temperatureAvg": 10.7,
            "uvIndexAvg": 1,
            "windSpeedAvg": 2.8,
            "windDirectionAvg": 157
          }
        },
        {
          "time": "2025-11-09T00:00:00Z",
          "values": {
            "cloudCoverAvg": 45,
            "temperatureApparentAvg": 12.5,
            "temperatureAvg": 12.5,
            "uvIndexAvg": 3,
            "windSpeedAvg": 4.2,
            "windDirectionAvg": 180
          }
        }
      ]
    }
  })";

  ForecastData data = { {}, 0, false };
  Logger::log("Initialized forecast data structure");

  // Check if forecastWeather object exists
  if (!forecastWeather) {
    Logger::log("ERROR: forecastWeather object is NULL!");
    return data;
  }

  Logger::log("forecastWeather object exists, calling parseForecastJson...");

  // Use the existing parsing function from WeatherForecast class
  if (!forecastWeather->parseForecastJson(String(testForecastJson), data)) {
    Logger::log("Failed to parse test forecast data");
    return data;
  }

  Logger::log("Test forecast data parsed successfully");
  Logger::log("Setting isValid to true...");
  data.isValid = true;
  Logger::log("=== Test forecast data loading complete ===");
  return data;
}

void initializeSystem() {
  Serial.begin(115200);

  Display::init();
}

void initializeOfflineMode() {
  timeManager = new TimeManager();
  // Don't sync NTP in offline mode, but initialize for time formatting

  realtimeWeather = new WeatherRealtime(String("test"), String("test"));
  forecastWeather = new WeatherForecast(String("test"), String("test"));
  poolTemperature = new PoolTemperature(timeManager);

  updateWeatherData();

  Display::updateSlideShow();

  delay(1000);
}

bool initializeWiFiConnection() {
  Logger::log("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 20) {
    delay(2000);
    Serial.print(".");
    connectionAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Logger::log("WiFi connected! IP address: ", WiFi.localIP().toString());
    Logger::log("Signal strength: " + String(WiFi.RSSI()) + " dBm");
    return true;
  }

  Serial.println();
  Logger::log("WiFi connection failed!");
  Logger::log("WiFi status: ", (int)WiFi.status());
  Display::displayError("WiFi connection failed");
  return false;
}

void initializeWeatherClients() {
  timeManager = new TimeManager();
  timeManager->begin();
  timeManager->syncWithNTP(); // Initial NTP sync

  realtimeWeather = new WeatherRealtime(API_KEY, LOCATION);
  forecastWeather = new WeatherForecast(API_KEY, LOCATION);
  poolTemperature = new PoolTemperature(timeManager);
  Logger::log("Fetching initial weather data...");
  updateWeatherData();
}

bool isUpdateRequired() {
  if (!offlineMode && WiFi.status() != WL_CONNECTED) {
    return false;
  }

  unsigned long currentTime = millis();
  return (currentTime - lastUpdate > updateIntervalMs);
}

void fetchAndDisplayRealtime() {
  Logger::log("=== Starting realtime weather fetch ===");
  RealtimeWeatherData realtimeData;

  if (offlineMode) {
    Logger::log("Using test data for realtime weather...");
    realtimeData = loadTestRealtimeData();
  }
  else {
    Logger::log("Fetching realtime weather...");
    realtimeData = realtimeWeather->fetchWeatherData();
  }

  Logger::log("Realtime data valid: ", realtimeData.isValid);

  if (!realtimeData.isValid) {
    Logger::log("Failed to get realtime weather data");
    Display::displayError("Failed to get current weather");
    return;
  }

  Logger::log("Realtime weather data received successfully");
  Logger::log("Calling Display::displayRealtimeWeather...");

  Display::displayRealtimeWeather(realtimeData);
  Logger::log("=== Realtime display call completed ===");
}

void fetchAndDisplayPoolTemp() {
  Logger::log("=== Starting pool temperature fetch ===");
  PoolTemperatureData poolData;

  if (offlineMode) {
    Logger::log("Using test data for pool temperature...");
    poolData = poolTemperature->loadTestData();
  }
  else {
    Logger::log("Fetching pool temperature...");
    poolData = poolTemperature->fetchPoolData();
  }

  Logger::log("Pool data valid: ", poolData.isValid);

  if (poolData.isValid) {
    Logger::log("Pool temperature: " + String(poolData.temperature) + "C");
    Logger::log("Time ago: " + poolData.timeAgo);
  }
  else {
    Logger::log("Failed to get pool temperature data");
  }

  // Update display with pool data (whether valid or not)
  Display::updatePoolData(poolData);
  Logger::log("=== Pool temperature fetch completed ===");
}

void fetchAndDisplayForecast() {
  Serial.println("=== Starting forecast weather fetch ===");
  ForecastData forecastData;

  if (offlineMode) {
    Serial.println("Using test data for forecast...");
    forecastData = loadTestForecastData();
  }
  else {
    Serial.println("Fetching weather forecast...");
    forecastData = forecastWeather->fetchForecastData();
  }

  Serial.print("Forecast data valid: ");
  Serial.println(forecastData.isValid ? "YES" : "NO");

  if (!forecastData.isValid) {
    Serial.println("Failed to get forecast data");
    Display::displayError("Failed to get forecast");
    return;
  }

  Serial.println("Forecast data received successfully");
  Serial.println("Calling Display::displayForecast...");
  Display::displayForecast(forecastData);
  Serial.println("=== Forecast display call completed ===");
}
