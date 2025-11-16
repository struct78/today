// TimeManager.h - Better time handling for Arduino projects
#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Logger.h"

class TimeManager {
private:
  static WiFiUDP udp;
  static unsigned long ntpSyncTime;
  static unsigned long ntpSyncMillis;
  static bool timeIsSynced;

  static const char* ntpServer;
  static const int ntpPort = 123;
  static const int NTP_PACKET_SIZE = 48;

public:
  static void begin() {
    udp.begin(8888);
    timeIsSynced = false;
    ntpSyncTime = 0;
    ntpSyncMillis = 0;
  }

  static bool syncWithNTP() {
    if (WiFi.status() != WL_CONNECTED) {
      Logger::log("WiFi not connected - cannot sync NTP");
      return false;
    }

    Logger::log("Syncing time with NTP server...");

    // Send NTP request
    if (!sendNTPRequest()) {
      Logger::log("Failed to send NTP request");
      return false;
    }

    // Wait for response with timeout
    unsigned long timeout = millis() + 5000; // 5 second timeout
    while (millis() < timeout) {
      if (udp.available()) {
        unsigned long ntpTime = parseNTPResponse();
        if (ntpTime > 0) {
          ntpSyncTime = ntpTime;
          ntpSyncMillis = millis();
          timeIsSynced = true;

          Logger::log("NTP sync successful. Unix time: " + String(ntpTime));
          return true;
        }
      }
      delay(100);
    }

    Logger::log("NTP sync timeout");
    return false;
  }

  static unsigned long getCurrentUnixTime() {
    if (!timeIsSynced) {
      // Return 0 if time is not synced
      return 0;
    }

    // Calculate current time based on NTP sync and elapsed millis
    unsigned long elapsedSeconds = (millis() - ntpSyncMillis) / 1000;
    return ntpSyncTime + elapsedSeconds;
  }

  static String formatTimeAgo(unsigned long timestamp) {
    if (timestamp == 0) {
      return "unknown";
    }

    unsigned long currentTime = getCurrentUnixTime();
    if (currentTime == 0) {
      // Fallback if NTP is not synced
      return formatRelativeTime(timestamp);
    }

    // Convert millisecond timestamp to seconds if needed
    unsigned long timestampSeconds = (timestamp > 1000000000000UL) ?
      timestamp / 1000 : timestamp;

    if (currentTime < timestampSeconds) {
      return "recently"; // Avoid negative time
    }

    unsigned long diffSeconds = currentTime - timestampSeconds;
    return formatDuration(diffSeconds);
  }

  static bool isTimeSynced() {
    return timeIsSynced;
  }

  static void resyncIfNeeded() {
    // Resync every 24 hours to account for clock drift
    const unsigned long resyncInterval = 24 * 60 * 60 * 1000; // 24 hours in ms

    if (timeIsSynced && (millis() - ntpSyncMillis > resyncInterval)) {
      Logger::log("Time sync is stale, re-syncing...");
      syncWithNTP();
    }
  }

private:
  static bool sendNTPRequest() {
    byte ntpBuffer[NTP_PACKET_SIZE] = { 0 };

    // Initialize NTP request packet
    ntpBuffer[0] = 0b11100011;   // LI, Version, Mode
    ntpBuffer[1] = 0;            // Stratum
    ntpBuffer[2] = 6;            // Polling interval
    ntpBuffer[3] = 0xEC;         // Peer Clock Precision

    udp.beginPacket(ntpServer, ntpPort);
    udp.write(ntpBuffer, NTP_PACKET_SIZE);
    return udp.endPacket();
  }

  static unsigned long parseNTPResponse() {
    if (udp.available() < NTP_PACKET_SIZE) {
      return 0;
    }

    byte ntpBuffer[NTP_PACKET_SIZE];
    udp.read(ntpBuffer, NTP_PACKET_SIZE);

    // Extract timestamp from bytes 40-43 (transmit timestamp)
    unsigned long timestamp = ((unsigned long)ntpBuffer[40] << 24) |
      ((unsigned long)ntpBuffer[41] << 16) |
      ((unsigned long)ntpBuffer[42] << 8) |
      (unsigned long)ntpBuffer[43];

    // Convert from NTP time (1900 epoch) to Unix time (1970 epoch)
    const unsigned long seventyYears = 2208988800UL;
    return timestamp - seventyYears;
  }

  static String formatDuration(unsigned long totalSeconds) {
    if (totalSeconds < 60) {
      return String(totalSeconds) + " seconds ago";
    }
    else if (totalSeconds < 3600) { // Less than 1 hour
      unsigned long minutes = totalSeconds / 60;
      return String(minutes) + (minutes == 1 ? " minute" : " minutes") + " ago";
    }
    else if (totalSeconds < 86400) { // Less than 1 day
      unsigned long hours = totalSeconds / 3600;
      unsigned long minutes = (totalSeconds % 3600) / 60;

      String result = String(hours) + (hours == 1 ? " hour" : " hours");
      if (minutes > 0) {
        result += " " + String(minutes) + (minutes == 1 ? " minute" : " minutes");
      }
      return result + " ago";
    }
    else if (totalSeconds < 604800) { // Less than 1 week
      unsigned long days = totalSeconds / 86400;
      unsigned long hours = (totalSeconds % 86400) / 3600;

      String result = String(days) + (days == 1 ? " day" : " days");
      if (hours > 0 && days < 7) { // Only show hours if less than a week
        result += " " + String(hours) + (hours == 1 ? " hour" : " hours");
      }
      return result + " ago";
    }
    else {
      unsigned long weeks = totalSeconds / 604800;
      return String(weeks) + (weeks == 1 ? " week" : " weeks") + " ago";
    }
  }

  static String formatRelativeTime(unsigned long timestamp) {
    // Fallback method when NTP is not available
    // This is less accurate but better than the original implementation

    // Try to estimate based on reasonable assumptions
    unsigned long currentEstimate = 1700000000UL; // Rough Nov 2023 baseline
    unsigned long bootSeconds = millis() / 1000;
    unsigned long estimatedNow = currentEstimate + bootSeconds;

    unsigned long timestampSeconds = (timestamp > 1000000000000UL) ?
      timestamp / 1000 : timestamp;

    if (estimatedNow < timestampSeconds) {
      return "recently";
    }

    unsigned long diff = estimatedNow - timestampSeconds;
    return formatDuration(diff);
  }
};

// Static member definitions
WiFiUDP TimeManager::udp;
unsigned long TimeManager::ntpSyncTime = 0;
unsigned long TimeManager::ntpSyncMillis = 0;
bool TimeManager::timeIsSynced = false;
const char* TimeManager::ntpServer = "au.pool.ntp.org";