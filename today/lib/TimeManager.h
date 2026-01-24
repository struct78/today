// TimeManager.h - Time handling using ezTime library
#pragma once

#include <Arduino.h>
#include <ezTime.h>
#include "Logger.h"
#include "../environment.h"

class TimeManager {
private:
  static Timezone localTz;
  static bool timeIsSynced;

public:
  static void begin() {
    timeIsSynced = false;

    // Set ezTime to use events (non-blocking)
    ezt::setInterval(60);  // Check NTP every 60 seconds if needed

    Logger::log("TimeManager initialized with timezone: " + String(TIMEZONE));
  }

  static bool syncWithNTP() {
    Logger::log("Syncing time with NTP server...");

    // Set debug level to see what's happening
    ezt::setDebug(INFO);

    // Try to sync with timeout using events() loop
    unsigned long startTime = millis();
    const unsigned long timeout = 15000; // 15 second timeout

    while (ezt::timeStatus() != timeSet) {
      ezt::events();
      delay(100);

      if (millis() - startTime > timeout) {
        Logger::log("NTP sync timeout after 15 seconds");
        Logger::log("Time status: " + String(ezt::timeStatus()));
        return false;
      }
    }

    Logger::log("NTP time synced, setting timezone...");

    // Set the timezone using POSIX string instead of network lookup
    // This avoids the timezoned.rop.nl network call
    localTz.setPosix(TIMEZONE_POSIX);

    timeIsSynced = true;
    Logger::log("NTP sync successful. Local time: " + localTz.dateTime());
    return true;
  }

  static unsigned long getCurrentUnixTime() {
    if (!timeIsSynced) {
      return 0;
    }
    return ezt::now();  // ezTime's UTC timestamp
  }

  static String formatTimeAgo(unsigned long timestamp) {
    if (timestamp == 0) {
      return "unknown";
    }

    unsigned long currentTime = getCurrentUnixTime();
    if (currentTime == 0) {
      return "unknown";
    }

    // Convert millisecond timestamp to seconds if needed
    unsigned long timestampSeconds = (timestamp > 1000000000000UL) ?
      timestamp / 1000 : timestamp;

    if (currentTime < timestampSeconds) {
      return "recently";
    }

    unsigned long diffSeconds = currentTime - timestampSeconds;
    return formatDuration(diffSeconds);
  }

  static bool isTimeSynced() {
    return timeIsSynced && ezt::timeStatus() == timeSet;
  }

  // Convert ISO8601 time string (e.g., "2024-01-15T14:00:00Z") to local hour (0-23)
  static int getLocalHourFromISO(const String& isoTime) {
    // Parse the ISO time string
    int year = isoTime.substring(0, 4).toInt();
    int month = isoTime.substring(5, 7).toInt();
    int day = isoTime.substring(8, 10).toInt();
    int hour = isoTime.substring(11, 13).toInt();
    int minute = isoTime.substring(14, 16).toInt();
    int second = isoTime.substring(17, 19).toInt();

    // Create UTC time using ezTime
    tmElements_t tm;
    tm.Year = year - 1970;  // ezTime uses years since 1970
    tm.Month = month;
    tm.Day = day;
    tm.Hour = hour;
    tm.Minute = minute;
    tm.Second = second;

    // Convert to Unix timestamp (UTC)
    time_t utcTime = ezt::makeTime(tm);

    // Convert to local time and return hour
    return localTz.hour(utcTime);
  }

  // Get current local hour (0-23)
  static int getCurrentLocalHour() {
    return localTz.hour();
  }

  // Get current local minute (0-59)
  static int getCurrentLocalMinute() {
    return localTz.minute();
  }

  // Format local time as HH:MM string
  static String formatLocalTime(unsigned long utcTimestamp) {
    if (utcTimestamp == 0) {
      return "--:--";
    }
    return localTz.dateTime(utcTimestamp, "H:i");
  }

  // Get current local time as formatted string
  static String getCurrentLocalTimeStr() {
    return localTz.dateTime("H:i");
  }

  // Check if DST is currently active
  static bool isDSTActive() {
    return localTz.isDST();
  }

  // Get current timezone abbreviation (e.g., "AEST" or "AEDT")
  static String getTimezoneAbbr() {
    return localTz.getTimezoneName();
  }

  static void resyncIfNeeded() {
    // ezTime handles this automatically, but we can trigger a check
    ezt::events();  // Process ezTime events
  }

private:
  static String formatDuration(unsigned long totalSeconds) {
    if (totalSeconds < 60) {
      return String(totalSeconds) + " seconds ago";
    }
    else if (totalSeconds < 3600) {
      unsigned long minutes = totalSeconds / 60;
      return String(minutes) + (minutes == 1 ? " minute" : " minutes") + " ago";
    }
    else if (totalSeconds < 86400) {
      unsigned long hours = totalSeconds / 3600;
      unsigned long minutes = (totalSeconds % 3600) / 60;

      String result = String(hours) + (hours == 1 ? " hour" : " hours");
      if (minutes > 0) {
        result += " " + String(minutes) + (minutes == 1 ? " minute" : " minutes");
      }
      return result + " ago";
    }
    else if (totalSeconds < 604800) {
      unsigned long days = totalSeconds / 86400;
      unsigned long hours = (totalSeconds % 86400) / 3600;

      String result = String(days) + (days == 1 ? " day" : " days");
      if (hours > 0 && days < 7) {
        result += " " + String(hours) + (hours == 1 ? " hour" : " hours");
      }
      return result + " ago";
    }
    else {
      unsigned long weeks = totalSeconds / 604800;
      return String(weeks) + (weeks == 1 ? " week" : " weeks") + " ago";
    }
  }
};

// Static member definitions
Timezone TimeManager::localTz;
bool TimeManager::timeIsSynced = false;
