// Logger.h
#pragma once
#include <Arduino.h>

class Logger {
public:
  // Simple message logging
  static void log(const String& message) {
    Serial.println(message.c_str());
  }

  // Key-value logging with string values
  static void log(const String& prefix, const String& value) {
    Serial.print(prefix.c_str());
    Serial.println(value.c_str());
  }

  // Key-value logging with integer values
  static void log(const String& prefix, int value) {
    Serial.print(prefix.c_str());
    Serial.println(value);
  }

  // Key-value logging with float values
  static void log(const String& prefix, float value) {
    Serial.print(prefix.c_str());
    Serial.println(value);
  }

  // Key-value logging with boolean values
  static void log(const String& prefix, bool value) {
    Serial.print(prefix.c_str());
    Serial.println(value ? "true" : "false");
  }

  // Raw print without newline
  static void print(const String& message) {
    Serial.print(message.c_str());
  }

  // Raw println with newline
  static void println(const String& message) {
    Serial.println(message.c_str());
  }

  // Print empty line
  static void println() {
    Serial.println();
  }

  // Enable/disable logging globally
  static void setEnabled(bool enabled) {
    loggingEnabled = enabled;
  }

  static bool isEnabled() {
    return loggingEnabled;
  }

private:
  static bool loggingEnabled;
};

// Static member definition
bool Logger::loggingEnabled = true;