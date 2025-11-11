#include <SPI.h>
#include <WiFi.h> // Re-enabled now that antenna is available
#include "credentials.h"
#include "lib/WeatherRealtime.h"
#include "lib/WeatherForecast.h"
#include "lib/Display.h"

// Global weather objects (re-enabled now that WiFi is available)
WeatherRealtime *realtimeWeather;
WeatherForecast *forecastWeather;

// Timing variables
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 300000; // 5 minutes between updates

void setup()
{
  Serial.begin(115200);
  Serial.println("Weather Station Starting...");
  Serial.println("Antenna connected - WiFi enabled");
  Serial.println("Touch screen to toggle display on/off");

  // Initialize the display (starts dark) with touch support
  Display::init();

  // Initialize WiFi connection
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 20)
  {
    delay(2000);
    Serial.print(".");
    connectionAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    // Initialize weather API clients
    realtimeWeather = new WeatherRealtime(API_KEY, LOCATION);
    forecastWeather = new WeatherForecast(API_KEY, LOCATION);

    // Get initial weather data
    Serial.println("Fetching initial weather data...");
    updateWeatherData();
  }
  else
  {
    Serial.println();
    Serial.println("WiFi connection failed!");
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());

    // Show error on display
    Display::displayError("WiFi connection failed");
  }

  delay(2000);
}

void loop()
{
  // Handle touch input for display toggle
  Display::handleTouchToggle();

  // Check if WiFi is connected and update weather data periodically
  if (WiFi.status() == WL_CONNECTED)
  {
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate > UPDATE_INTERVAL)
    {
      Serial.println("Updating weather data...");
      updateWeatherData();
      lastUpdate = currentTime;
    }
  }

  // Small delay to prevent excessive CPU usage
  delay(100);
}

void updateWeatherData()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected - skipping weather update");
    return;
  }

  // Get realtime weather data
  Serial.println("Fetching realtime weather...");
  RealtimeWeatherData realtimeData = realtimeWeather->fetchWeatherData();

  if (realtimeData.isValid)
  {
    Serial.println("Realtime weather data received successfully");
    Display::displayRealtimeWeather(realtimeData);
  }
  else
  {
    Serial.println("Failed to get realtime weather data");
    Display::displayError("Failed to get current weather");
  }

  // Get forecast data
  Serial.println("Fetching weather forecast...");
  ForecastData forecastData = forecastWeather->fetchForecastData();

  if (forecastData.isValid)
  {
    Serial.println("Forecast data received successfully");
    Display::displayForecast(forecastData);
  }
  else
  {
    Serial.println("Failed to get forecast data");
    Display::displayError("Failed to get forecast");
  }
}
