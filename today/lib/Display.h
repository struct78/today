// Display.h
#pragma once
#include <Arduino.h>
#include "Logger.h"
#include "WeatherRealtime.h"
#include "WeatherForecast.h"
#include "PoolTemperature.h"
#include <Arduino_GigaDisplay.h>
#include "Arduino_GigaDisplay_GFX.h"
#include "Arduino_GigaDisplayTouch.h"
#include "WeatherIcons.h"
#include "./fonts/InterBold18pt.h"
#include "./fonts/InterMedium24pt.h"
#include "./fonts/InterRegular12pt.h"

// Color definitions for 16-bit (565) RGB
// Basic Colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0

// Grayscale Colors
#define DARK_GRAY 0x2104
#define GRAY 0x4208
#define LIGHT_GRAY 0x630C
#define SILVER 0x7BEF
#define LIGHT_SILVER 0x9CF3

// Red Variants
#define DARK_RED 0x8800
#define RED_ORANGE 0xC800
#define LIGHT_RED 0xF8C0
#define BRIGHT_RED 0xFC00

// Green Variants
#define DARK_GREEN 0x0400
#define FOREST_GREEN 0x0600
#define LIGHT_GREEN 0x87E0
#define LIME_GREEN 0xAFE0

// Blue Variants
#define DARK_BLUE 0x0010
#define NAVY_BLUE 0x0015
#define LIGHT_BLUE 0x841F
#define SKY_BLUE 0xC61F

// Orange/Brown Colors
#define ORANGE 0xFD20
#define DARK_ORANGE 0xFC00
#define LIGHT_ORANGE 0xFBE0
#define BROWN 0xA145
#define LIGHT_BROWN 0xD343

// Purple/Violet Colors
#define PURPLE 0x780F
#define DARK_PURPLE 0xA817
#define LIGHT_MAGENTA 0xFC1F
#define DARK_VIOLET 0x8010

// Pink Colors
#define PINK 0xFC9F
#define LIGHT_PINK 0xFE19
#define PALE_PINK 0xFDF9

// Nature Colors
#define OLIVE 0x2589
#define DARK_OLIVE 0x4D69
#define KHAKI 0x6D2D
#define SEA_GREEN 0x3C69
#define DARK_SEA_GREEN 0x5D8A

// Weather App Utility Colors
#define MIDNIGHT_BLUE 0x051D
#define DEEP_SKY_BLUE 0x3C9F
#define CLOUD_GRAY 0x39E7
#define SUN_YELLOW 0xFEA0
#define STORM_GRAY 0x4208

// Pool/Water Colors
#define TURQUOISE 0x4F9B
#define TEAL 0x2B5A
#define STEEL_BLUE 0x4C9F
#define DARK_SLATE_BLUE 0x2B75
#define WARM_ORANGE 0xFD00
#define FOG_GRAY 0x8C51

class Display {
private:
  static GigaDisplay_GFX display;
  static Arduino_GigaDisplayTouch touch;
  static GigaDisplayBacklight backlight;
  static int currentY;
  static const int lineHeight = 25;
  static const int marginY = 50;
  static const int marginX = 30;
  static bool displayOn;
  static unsigned long lastTouchTime;
  static bool touchInProgress;                        // Track if touch is currently active
  static const unsigned long touchDebounceTimeMs = 200; // 2 second debounce to prevent rapid toggling

  // Slide cycling variables
  static unsigned long lastSlideChange;
  static int currentSlide;
  static const unsigned long slideDuration = 7000;
  static const int totalSlides = 6;
  static RealtimeWeatherData currentWeatherData;
  static PoolTemperatureData currentPoolData;

private:
  static void drawWeatherIcon(int centerX, int centerY) {
    // Draw sun (yellow circle with rays)
    int sunX = centerX - 20;
    int sunY = centerY - 15;
    int sunRadius = 25;

    // Sun body
    display.fillCircle(sunX, sunY, sunRadius, YELLOW);

    // Sun rays
    int rayLength = 15;
    int rayDistance = sunRadius + 5;

    // Vertical rays
    display.drawLine(sunX, sunY - rayDistance, sunX, sunY - rayDistance - rayLength, YELLOW);
    display.drawLine(sunX, sunY + rayDistance, sunX, sunY + rayDistance + rayLength, YELLOW);

    // Horizontal rays
    display.drawLine(sunX - rayDistance, sunY, sunX - rayDistance - rayLength, sunY, YELLOW);
    display.drawLine(sunX + rayDistance, sunY, sunX + rayDistance + rayLength, sunY, YELLOW);

    // Diagonal rays
    display.drawLine(sunX + 18, sunY - 18, sunX + 18 + 12, sunY - 18 - 12, YELLOW);
    display.drawLine(sunX - 18, sunY - 18, sunX - 18 - 12, sunY - 18 - 12, YELLOW);
    display.drawLine(sunX + 18, sunY + 18, sunX + 18 + 12, sunY + 18 + 12, YELLOW);
    display.drawLine(sunX - 18, sunY + 18, sunX - 18 - 12, sunY + 18 + 12, YELLOW);

    // Draw cloud (white/gray overlapping circles)
    int cloudX = centerX + 15;
    int cloudY = centerY + 10;

    // Draw light grey stroke outlines first (unfilled circles)
    display.drawCircle(cloudX - 16, cloudY - 1, 19, DEEP_SKY_BLUE); // Left circle outline
    display.drawCircle(cloudX - 1, cloudY - 9, 23, DEEP_SKY_BLUE);  // Center circle outline
    display.drawCircle(cloudX + 16, cloudY - 1, 19, DEEP_SKY_BLUE); // Right circle outline
    display.drawCircle(cloudX + 26, cloudY + 6, 16, DEEP_SKY_BLUE); // Right extension outline

    // Cloud circles (create puffy cloud shape) - filled white circles on top
    display.fillCircle(cloudX - 15, cloudY, 18, WHITE);     // Left circle
    display.fillCircle(cloudX, cloudY - 8, 22, WHITE);      // Center circle
    display.fillCircle(cloudX + 15, cloudY, 18, WHITE);     // Right circle
    display.fillCircle(cloudX + 25, cloudY + 5, 15, WHITE); // Right extension

    // Add text
    display.setTextSize(3);
    display.setTextColor(WHITE);

    int textX = centerX - 45;
    int textY = centerY + 70;

    display.setCursor(textX, textY);
    display.print("TODAY");

    resetTextSize();
  }

  // Icon drawing functions for different weather parameters
  static void drawTemperatureIcon(int centerX, int centerY) {
    // Draw a thermometer (tripled size - doubled + 50%)
    int thermX = centerX;
    int thermY = centerY;

    // Thermometer bulb (circle at bottom) - tripled radius
    display.fillCircle(thermX, thermY + 60, 24, RED);

    // Thermometer stem (rectangle) - tripled dimensions
    display.fillRect(thermX - 9, thermY - 60, 18, 120, WHITE);
    display.drawRect(thermX - 9, thermY - 60, 18, 120, GRAY);

    // Temperature markings - tripled spacing and length
    for (int i = 0; i < 4; i++) {
      int markY = thermY - 45 + (i * 24);
      display.drawLine(thermX + 9, markY, thermX + 24, markY, GRAY);
    }
  }

  static void drawHumidityIcon(int centerX, int centerY) {
    // Draw a water droplet (tripled size - doubled + 50%)
    int dropX = centerX;
    int dropY = centerY;

    // Water droplet shape using circles and triangle approximation - tripled radius
    display.fillCircle(dropX, dropY + 15, 36, BLUE);
    display.fillTriangle(dropX, dropY - 30, dropX - 24, dropY + 15, dropX + 24, dropY + 15, BLUE);

    // Highlight for shine effect - tripled radius
    display.fillCircle(dropX - 12, dropY, 9, LIGHT_BLUE);
  }

  static void drawWindIcon(int centerX, int centerY) {
    // Draw wind lines (tripled size - doubled + 50%)
    int windX = centerX;
    int windY = centerY;

    // Multiple curved wind lines - tripled dimensions
    for (int i = 0; i < 3; i++) {
      int lineY = windY - 30 + (i * 24);
      // Horizontal lines with slight curves - tripled length
      display.drawLine(windX - 45, lineY, windX + 30, lineY, WHITE);
      display.drawLine(windX + 30, lineY, windX + 45, lineY - 6, WHITE);
      display.drawLine(windX + 45, lineY - 6, windX + 36, lineY - 15, WHITE);
    }
  }

  static void drawCloudIcon(int centerX, int centerY) {
    // Draw a cloud (tripled size - doubled + 50%)
    int cloudX = centerX;
    int cloudY = centerY;

    // Cloud circles (create puffy cloud shape) - tripled radius
    display.fillCircle(cloudX - 36, cloudY, 45, WHITE);
    display.fillCircle(cloudX, cloudY - 24, 54, WHITE);
    display.fillCircle(cloudX + 36, cloudY, 45, WHITE);
    display.fillCircle(cloudX + 60, cloudY + 15, 36, WHITE);

    // Outline for definition - tripled radius
    display.drawCircle(cloudX - 36, cloudY, 45, LIGHT_GRAY);
    display.drawCircle(cloudX, cloudY - 24, 54, LIGHT_GRAY);
    display.drawCircle(cloudX + 36, cloudY, 45, LIGHT_GRAY);
    display.drawCircle(cloudX + 60, cloudY + 15, 36, LIGHT_GRAY);
  }

  static void drawUVIcon(int centerX, int centerY) {
    // Draw a sun with UV rays (tripled size - doubled + 50%)
    int sunX = centerX;
    int sunY = centerY;

    // Sun body - tripled radius
    display.fillCircle(sunX, sunY, 45, YELLOW);

    // Sun rays (longer and more prominent for UV) - tripled length
    int rayLength = 60;
    int rayDistance = 54;

    // 8 rays in all directions
    for (int i = 0; i < 8; i++) {
      float angle = i * 45.0 * PI / 180.0;
      int startX = sunX + cos(angle) * rayDistance;
      int startY = sunY + sin(angle) * rayDistance;
      int endX = sunX + cos(angle) * (rayDistance + rayLength);
      int endY = sunY + sin(angle) * (rayDistance + rayLength);

      display.drawLine(startX, startY, endX, endY, ORANGE);
      display.drawLine(startX, startY + 1, endX, endY + 1, ORANGE); // Thicker lines
      display.drawLine(startX + 1, startY, endX + 1, endY, ORANGE); // Even thicker lines
    }
  }

  static void drawSwimmingIcon(int centerX, int centerY) {
    // Draw swimming person icon (🏊‍♂️ emoji representation)
    int swimmerX = centerX;
    int swimmerY = centerY;

    // Draw water waves at bottom (tripled size)
    int waveY = swimmerY + 40;
    for (int i = 0; i < 3; i++) {
      int waveBaseY = waveY + (i * 12);
      // Wavy lines using multiple short segments
      for (int x = -90; x <= 90; x += 15) {
        int y1 = waveBaseY + (int)(6 * sin(x * 0.1));
        int y2 = waveBaseY + (int)(6 * sin((x + 15) * 0.1));
        display.drawLine(swimmerX + x, y1, swimmerX + x + 15, y2, CYAN);
        display.drawLine(swimmerX + x, y1 + 1, swimmerX + x + 15, y2 + 1, CYAN);
      }
    }

    // Draw swimmer's head (circle) - tripled radius
    display.fillCircle(swimmerX + 30, swimmerY - 30, 18, WHITE);
    display.drawCircle(swimmerX + 30, swimmerY - 30, 18, GRAY);

    // Draw swimmer's body (oval/rectangle) - tripled dimensions
    display.fillRect(swimmerX - 15, swimmerY - 12, 45, 24, WHITE);
    display.drawRect(swimmerX - 15, swimmerY - 12, 45, 24, GRAY);

    // Draw extended arm (swimming stroke) - tripled length
    display.drawLine(swimmerX - 15, swimmerY - 6, swimmerX - 60, swimmerY - 24, WHITE);
    display.drawLine(swimmerX - 15, swimmerY - 6, swimmerX - 60, swimmerY - 21, WHITE);
    display.drawLine(swimmerX - 15, swimmerY - 6, swimmerX - 60, swimmerY - 18, WHITE);

    // Hand
    display.fillCircle(swimmerX - 60, swimmerY - 21, 6, WHITE);

    // Draw legs (kicking) - tripled length
    display.drawLine(swimmerX + 30, swimmerY + 12, swimmerX + 60, swimmerY - 6, WHITE);
    display.drawLine(swimmerX + 30, swimmerY + 12, swimmerX + 60, swimmerY - 3, WHITE);
    display.drawLine(swimmerX + 30, swimmerY + 12, swimmerX + 60, swimmerY, WHITE);

    display.drawLine(swimmerX + 30, swimmerY + 12, swimmerX + 54, swimmerY + 30, WHITE);
    display.drawLine(swimmerX + 30, swimmerY + 12, swimmerX + 57, swimmerY + 30, WHITE);
    display.drawLine(swimmerX + 30, swimmerY + 12, swimmerX + 60, swimmerY + 30, WHITE);
  }

private:
  // Function to get background color based on weather parameter and value
  static uint16_t getBackgroundColor(const String& paramType, float value) {
    if (paramType == "temperature") {
      if (value >= 30) return RED_ORANGE;          // Hot - orange/red
      else if (value >= 20) return FOREST_GREEN;   // Warm - green
      else if (value >= 15) return DEEP_SKY_BLUE;  // Cool - blue
      else return DARK_BLUE;                       // Cold - dark blue
    }
    else if (paramType == "pool") {
      if (value >= 25) return TURQUOISE;           // Warm pool - turquoise
      else if (value >= 20) return TEAL;           // Comfortable - teal
      else if (value >= 15) return STEEL_BLUE;     // Cool pool - steel blue
      else return DARK_SLATE_BLUE;                 // Cold pool - dark blue
    }
    else if (paramType == "uv") {
      if (value >= 8) return RED;                  // Very high UV - red
      else if (value >= 6) return ORANGE;          // High UV - orange
      else if (value >= 3) return YELLOW;          // Moderate UV - yellow
      else return DARK_SEA_GREEN;                     // Low UV - light green
    }
    else if (paramType == "humidity") {
      return NAVY_BLUE;                            // Static blue for humidity
    }
    else if (paramType == "wind") {
      return STORM_GRAY;                           // Gray for wind
    }
    else if (paramType == "cloud") {
      return CLOUD_GRAY;                           // Gray for cloud cover
    }

    return DEEP_SKY_BLUE; // Default background
  }

  static void resetTextSize() {
    display.setTextSize(2);
  }

public:
  static void init() {
    Logger::log("=== Display::init() starting ===");

    display.begin();
    touch.begin();
    backlight.begin();

    display.setRotation(1);
    display.fillScreen(DEEP_SKY_BLUE);

    currentY = marginY;
    displayOn = true;
    lastTouchTime = 0;
    touchInProgress = false;

    drawWeatherIcon(display.width() / 2 - 20, display.height() / 2 - 20);
  }

  static void setBacklight(bool on) {
    if (on) {
      // Turn on display
      backlight.set(100);
      currentY = marginY;
    }
    else {
      backlight.set(0);
    }
    displayOn = on;
  }

  static bool checkTouch() {
    unsigned long currentTime = millis();

    GDTpoint_t points[5];
    uint8_t contacts = touch.getTouchPoints(points);

    if (contacts > 0) {
      // Touch is currently detected
      if (!touchInProgress) {
        // This is a new touch (finger just pressed down)
        // Debounce check
        if (currentTime - lastTouchTime < touchDebounceTimeMs) {
          return false;
        }

        // Get the first touch point coordinates
        int x = points[0].x;
        int y = points[0].y;

        lastTouchTime = currentTime;
        touchInProgress = true;

        Logger::log("Touch press detected at: (" + String(x) + ", " + String(y) + ")");
        return true;
      }
      // Touch is still ongoing, don't trigger again
      return false;
    }
    else {
      // No touch detected - reset touch in progress flag
      touchInProgress = false;
      return false;
    }
  }

  static void handleTouchToggle() {
    if (checkTouch()) {
      displayOn = !displayOn;
      setBacklight(displayOn);
    }
  }

  static void refreshDisplay() {
    if (!displayOn) {
      return;
    }

    clearScreen();
  }

  static bool isDisplayOn() {
    return displayOn;
  }

  static void clearScreen() {
    // Always clear when explicitly called, regardless of display state
    display.fillScreen(BLACK);
    currentY = marginY;
  }

  static void printLine(const String& text, uint16_t color = WHITE) {
    if (!displayOn)
      return; // Don't print if display is off

    display.setTextColor(color);
    display.setCursor(marginY, currentY);
    display.print(text);
    currentY += lineHeight;

    // Wrap to top if we've reached the bottom
    if (currentY > display.height() - lineHeight) {
      currentY = marginY;
    }
  }

  static void displaySlide(const String& title, const String& value, const String& unit = "", const String& iconType = "") {
    Logger::log("=== displaySlide() called ===");
    Logger::log("displayOn: ", displayOn);
    Logger::log("Title: ", title);
    Logger::log("Value: ", value);

    if (!displayOn) {
      Logger::log("Display is OFF, not showing slide");
      return;
    }

    // Determine background color based on parameter type and value
    uint16_t backgroundColor = DEEP_SKY_BLUE; // Default
    if (iconType.length() > 0) {
      float numericValue = value.toFloat();

      if (iconType == "temperature") {
        backgroundColor = getBackgroundColor("temperature", numericValue);
      }
      else if (iconType == "pool") {
        backgroundColor = getBackgroundColor("pool", numericValue);
      }
      else if (iconType == "uv") {
        backgroundColor = getBackgroundColor("uv", numericValue);
      }
      else if (iconType == "humidity") {
        backgroundColor = getBackgroundColor("humidity", numericValue);
      }
      else if (iconType == "wind") {
        backgroundColor = getBackgroundColor("wind", numericValue);
      }
      else if (iconType == "cloud") {
        backgroundColor = getBackgroundColor("cloud", numericValue);
      }
    }

    // Clear screen with dynamic background color
    display.fillScreen(backgroundColor);

    // Display title at top left with enhanced styling using Inter font
    display.setFont(&Inter_Regular12pt7b);
    display.setTextColor(WHITE);
    display.setCursor(marginX, marginY + 20);
    display.print(title.c_str());

    // Display icon in center if specified
    if (iconType.length() > 0) {
      int centerX = display.width() / 2;
      int centerY = display.height() / 2;

      if (iconType == "temperature") {
        drawTemperatureIcon(centerX, centerY);
      }
      else if (iconType == "pool") {
        drawSwimmingIcon(centerX, centerY);
      }
      else if (iconType == "humidity") {
        drawHumidityIcon(centerX, centerY);
      }
      else if (iconType == "wind") {
        drawWindIcon(centerX, centerY);
      }
      else if (iconType == "cloud") {
        drawCloudIcon(centerX, centerY);
      }
      else if (iconType == "uv") {
        drawUVIcon(centerX, centerY);
      }
    }

    // Display value at bottom left with largest font size using Inter font
    display.setFont(&Inter_Medium24pt7b);
    int valueY = display.height() - marginY;
    display.setCursor(marginX, valueY);
    display.print((value + unit).c_str());

    resetTextSize();
    Logger::log("=== displaySlide() completed ===");
  }

  static void updateSlideShow() {
    if (!displayOn || !currentWeatherData.isValid) {
      if (!displayOn) Logger::log("Display is OFF");
      if (!currentWeatherData.isValid) Logger::log("Weather data is INVALID");
      return;
    }

    unsigned long currentTime = millis();

    // Check if it's time to change slides
    if (currentTime - lastSlideChange >= slideDuration) {
      currentSlide = (currentSlide + 1) % totalSlides;
      lastSlideChange = currentTime;

      Logger::log("Switching to slide " + String(currentSlide) + "...");

      // Display the current slide with appropriate icons
      switch (currentSlide) {
      case 0: // Temperature
        displaySlide("Temperature", String(currentWeatherData.temperature, 1), "C", "temperature");
        break;
      case 1: // UV Index
        displaySlide("UV Index", String(currentWeatherData.uvIndex), "", "uv");
        break;
      case 2: // Humidity
        displaySlide("Humidity", String(currentWeatherData.humidity, 1), "%", "humidity");
        break;
      case 3: // Wind Speed
        displaySlide("Wind Speed", String(currentWeatherData.windSpeed, 1), "km/h", "wind");
        break;
      case 4: // Cloud Cover
        displaySlide("Cloud Cover", String(currentWeatherData.cloudCover), "%", "cloud");
        break;
      case 5: // Pool Temperature
        if (currentPoolData.isValid) {
          displaySlide("Pool Temp", String(currentPoolData.temperature, 1), "C", "pool");
        }
        else {
          displaySlide("Pool Temp", "No data", "", "pool");
        }
        break;
      default:
        break;
      }

      Logger::log("Displaying slide " + String(currentSlide + 1) + " of " + String(totalSlides));
    }
  }

  static void startSlideShow(const RealtimeWeatherData& data) {
    Logger::log("=== startSlideShow() called ===");
    Logger::log("data.isValid: ", data.isValid);
    Logger::log("displayOn: ", displayOn);

    currentWeatherData = data;
    currentSlide = -1;
    lastSlideChange = millis();

    if (!data.isValid) {
      displaySlide("Error", "Load failed");
    }
  }

  static void displayRealtimeWeather(const RealtimeWeatherData& data) {
    Logger::log("DisplayOn: ", displayOn);

    if (!displayOn) {
      return;
    }

    if (!data.isValid) {
      displayError("No realtime data available");
      return;
    }

    // Start the slideshow instead of displaying static data
    startSlideShow(data);
  }

  static void displayForecast(const ForecastData& data) {
    if (!displayOn)
      return; // Don't display if screen is off

    printLine("=== 7-DAY FORECAST ===", CYAN);

    if (!data.isValid || data.dayCount == 0) {
      printLine("No forecast data available", RED);
      return;
    }

    // Display only first 3 days due to screen space
    int displayDays = min(data.dayCount, 3);

    for (int i = 0; i < displayDays; i++) {
      const DailyForecastData& day = data.daily[i];

      if (!day.isValid)
        continue;

      printLine("Day " + String(i + 1), YELLOW);
      printLine("Temp: " + String(day.temperatureAvg) + "C");
      printLine("UV: " + String(day.uvIndexAvg) + " Cloud: " + String(day.cloudCoverAvg) + "%");
      printLine("Wind: " + String(day.windSpeedAvg) + " km/h " + String(day.windDirectionAvg) + " deg");
      printLine(""); // Empty line between days
    }
  }

  static void displayError(const String& message) {
    if (!displayOn)
      return; // Don't display if screen is off

    printLine("=== ERROR ===", RED);
    printLine(message, RED);
    printLine("", RED);
  }

  static void updatePoolData(const PoolTemperatureData& poolData) {
    Logger::log("=== updatePoolData() called ===");
    Logger::log("poolData.isValid: ", poolData.isValid);

    currentPoolData = poolData;

    if (poolData.isValid) {
      Logger::log("Pool temp: " + String(poolData.temperature) + "C, " + poolData.timeAgo);
    }
  }
};

// Static member definitions
GigaDisplay_GFX Display::display;
Arduino_GigaDisplayTouch Display::touch;
GigaDisplayBacklight Display::backlight;
int Display::currentY = 10;
bool Display::displayOn = false;
unsigned long Display::lastTouchTime = 0;
bool Display::touchInProgress = false;

// Slide cycling static member definitions
unsigned long Display::lastSlideChange = 0;
int Display::currentSlide = 0;
RealtimeWeatherData Display::currentWeatherData = { 0, 0, 0, 0, 0, false };
PoolTemperatureData Display::currentPoolData = { "", 0.0f, 0, "", false };
