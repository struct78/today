// Display.h
#pragma once
#include <Arduino.h>
#include "WeatherRealtime.h"
#include "WeatherForecast.h"
#include <Arduino_GigaDisplay.h>
#include "Arduino_GigaDisplay_GFX.h"
#include "Arduino_GigaDisplayTouch.h"
#include "WeatherIcons.h"

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
#define WARM_ORANGE 0xFD00
#define STORM_GRAY 0x2589
#define FOG_GRAY 0x8C51

class Display {
private:
  static GigaDisplay_GFX display;
  static Arduino_GigaDisplayTouch touch;
  static GigaDisplayBacklight backlight;
  static int currentY;
  static const int lineHeight = 25;
  static const int margin = 10;
  static bool displayOn;
  static unsigned long lastTouchTime;
  static bool touchInProgress;                        // Track if touch is currently active
  static const unsigned long touchDebounceTime = 200; // 2 second debounce to prevent rapid toggling

private:
  static void drawWeatherIcon(int centerX, int centerY) {
    // Draw a custom pixel-based weather icon (sun with cloud) 🌤️

    // Draw sun (yellow circle with rays)
    int sunX = centerX - 20;
    int sunY = centerY - 15;
    int sunRadius = 25;

    // Sun body (filled circle)
    display.fillCircle(sunX, sunY, sunRadius, YELLOW);

    // Sun rays (8 directional lines)
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

    display.setTextSize(3);
    display.setTextColor(WHITE);

    int textX = centerX - 45;
    int textY = centerY + 70;

    display.setCursor(textX, textY);
    display.print("TODAY");

    resetTextSize();
  }

private:
  static void resetTextSize() {
    display.setTextSize(2);
  }

public:
  static void init() {
    display.begin();
    touch.begin(); // Initialize touch interface
    backlight.begin();

    // Set display rotation for landscape mode (90 degrees)
    display.setRotation(1); // 0=portrait, 1=landscape, 2=portrait flipped, 3=landscape flipped
    display.setTextColor(WHITE);
    display.setTextSize(2);
    currentY = margin;
    displayOn = false; // Start with display off (dark)
    lastTouchTime = 0;
    touchInProgress = false; // Initialize touch state

    // Draw custom weather icon in center of display
    int centerX = display.width() / 2;
    int centerY = display.height() / 2 - 20;

    drawWeatherIcon(centerX, centerY);
  }

  static void setBacklight(bool on) {
    if (on) {
      // Turn on display
      backlight.set(100);
      currentY = margin;
    }
    else {
      backlight.set(0);
    }
    displayOn = on;
  }

  static bool checkTouch() {
    // Check if screen is touched using the proper Arduino_GigaDisplayTouch library
    unsigned long currentTime = millis();

    // Check for touch using the GigaDisplayTouch library
    GDTpoint_t points[5]; // Support up to 5 touch points
    uint8_t contacts = touch.getTouchPoints(points);

    if (contacts > 0) {
      // Touch is currently detected
      if (!touchInProgress) {
        // This is a new touch (finger just pressed down)
        // Debounce check
        if (currentTime - lastTouchTime < touchDebounceTime) {
          return false;
        }

        // Get the first touch point coordinates
        int x = points[0].x;
        int y = points[0].y;

        lastTouchTime = currentTime;
        touchInProgress = true;

        Serial.print("Touch press detected at: (");
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.println(")");
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
    currentY = margin;
  }

  static void printLine(const String& text, uint16_t color = WHITE) {
    if (!displayOn)
      return; // Don't print if display is off

    display.setTextColor(color);
    display.setCursor(margin, currentY);
    display.print(text);
    currentY += lineHeight;

    // Wrap to top if we've reached the bottom
    if (currentY > display.height() - lineHeight) {
      currentY = margin;
    }
  }

  static void displayRealtimeWeather(const RealtimeWeatherData& data) {
    Serial.print("DisplayOn");
    Serial.println(displayOn);

    if (!displayOn)
      return; // Don't display if screen is off

    printLine("=== CURRENT WEATHER ===", CYAN);

    if (!data.isValid) {
      printLine("No realtime data available", RED);
      return;
    }

    // Add weather icons alongside data
    int iconX = display.width() - 60;
    int iconY = currentY + 10;

    WeatherIcons::drawThermometerIcon(display, iconX, iconY, YELLOW);
    printLine("UV Index: " + String(data.uvIndex));

    WeatherIcons::drawUVIcon(display, iconX, iconY + 25, YELLOW);
    printLine("Humidity: " + String(data.humidity) + "%");

    WeatherIcons::drawHumidityIcon(display, iconX, iconY + 50, CYAN);
    printLine("Wind Speed: " + String(data.windSpeed) + " km/h");

    WeatherIcons::drawWindIcon(display, iconX, iconY + 75, WHITE);
    printLine("Wind Direction: " + String(data.windDirection) + "°");

    WeatherIcons::drawCloudIcon(display, iconX, iconY + 100, GRAY);
    printLine("Cloud Cover: " + String(data.cloudCover) + "%");
    printLine(""); // Empty line
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
      printLine("Temp: " + String(day.temperatureAvg) + "°C");
      printLine("UV: " + String(day.uvIndexAvg) + " Cloud: " + String(day.cloudCoverAvg) + "%");
      printLine("Wind: " + String(day.windSpeedAvg) + " km/h " + String(day.windDirectionAvg) + "°");
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
};

// Static member definitions
GigaDisplay_GFX Display::display;
Arduino_GigaDisplayTouch Display::touch;
GigaDisplayBacklight Display::backlight;
int Display::currentY = 10;
bool Display::displayOn = false;
unsigned long Display::lastTouchTime = 0;
bool Display::touchInProgress = false;
