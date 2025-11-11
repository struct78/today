// WeatherIcons.h - Simple weather icon functions for Arduino Giga
#pragma once
#include "Arduino_GigaDisplay_GFX.h"

class WeatherIcons {
public:
  // Simple weather icons using basic shapes
  static void drawSunIcon(GigaDisplay_GFX& display, int16_t x, int16_t y, uint16_t color) {
    // Sun - filled circle with rays
    display.fillCircle(x, y, 15, color);
    // Sun rays
    for (int i = 0; i < 8; i++) {
      float angle = (i * 45) * PI / 180;
      int16_t x1 = x + cos(angle) * 20;
      int16_t y1 = y + sin(angle) * 20;
      int16_t x2 = x + cos(angle) * 25;
      int16_t y2 = y + sin(angle) * 25;
      display.drawLine(x1, y1, x2, y2, color);
    }
  }

  static void drawCloudIcon(GigaDisplay_GFX& display, int16_t x, int16_t y, uint16_t color) {
    // Cloud - multiple overlapping circles
    display.fillCircle(x - 10, y, 8, color);
    display.fillCircle(x, y - 5, 12, color);
    display.fillCircle(x + 10, y, 10, color);
    display.fillCircle(x + 5, y + 5, 8, color);
  }

  static void drawRainIcon(GigaDisplay_GFX& display, int16_t x, int16_t y, uint16_t color) {
    // Cloud + rain drops
    drawCloudIcon(display, x, y - 10, color);
    // Rain drops
    for (int i = 0; i < 5; i++) {
      int16_t dropX = x - 15 + (i * 7);
      display.drawLine(dropX, y + 5, dropX, y + 15, color);
    }
  }

  static void drawWindIcon(GigaDisplay_GFX& display, int16_t x, int16_t y, uint16_t color) {
    // Wind - curved lines
    display.drawLine(x - 15, y - 5, x + 15, y - 5, color);
    display.drawLine(x - 10, y, x + 10, y, color);
    display.drawLine(x - 5, y + 5, x + 5, y + 5, color);
  }

  static void drawThermometerIcon(GigaDisplay_GFX& display, int16_t x, int16_t y, uint16_t color) {
    // Thermometer - rectangle with circle at bottom
    display.drawRect(x - 2, y - 15, 4, 20, color);
    display.fillCircle(x, y + 8, 6, color);
  }

  static void drawHumidityIcon(GigaDisplay_GFX& display, int16_t x, int16_t y, uint16_t color) {
    // Water drop shape using triangle and circle
    display.drawTriangle(x, y - 10, x - 8, y + 5, x + 8, y + 5, color);
    display.fillCircle(x, y + 2, 6, color);
  }

  static void drawUVIcon(GigaDisplay_GFX& display, int16_t x, int16_t y, uint16_t color) {
    // UV rays from center
    for (int i = 0; i < 12; i++) {
      float angle = (i * 30) * PI / 180;
      int16_t x1 = x + cos(angle) * 8;
      int16_t y1 = y + sin(angle) * 8;
      int16_t x2 = x + cos(angle) * 15;
      int16_t y2 = y + sin(angle) * 15;
      display.drawLine(x1, y1, x2, y2, color);
    }
    display.fillCircle(x, y, 4, color);
  }
};