#!/bin/bash

# WiFi Network Scanner for macOS
# Shows SSID, RSSI, Channel, Frequency, and Security

echo "🔍 WiFi Network Scanner"
echo "=================================================="

# Method 1: Using system_profiler (most reliable)
echo "📡 Networks detected by macOS:"
system_profiler SPAirPortDataType | grep -A 10 "Other Local Wi-Fi Networks:" | \
    grep -E "(PHY Mode|Channel|Security|Signal)" | \
    sed 's/^[ \t]*//' | \
    while IFS= read -r line; do
        if [[ $line == *":"* ]] && [[ $line != *"PHY Mode"* ]] && [[ $line != *"Channel"* ]] && [[ $line != *"Security"* ]] && [[ $line != *"Signal"* ]]; then
            echo "SSID: ${line%:*}"
        else
            echo "  $line"
        fi
    done

echo ""
echo "=================================================="
echo "🔧 Alternative Methods:"
echo ""

# Method 2: Try airport command (deprecated but works)
echo "📊 Raw airport scan (if available):"
if [[ -f "/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport" ]]; then
    /System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s 2>/dev/null | head -10
else
    echo "  Airport utility not found"
fi

echo ""
echo "=================================================="
echo "📋 Command Summary:"
echo ""
echo "� Scan networks:"
echo "  system_profiler SPAirPortDataType"
echo ""
echo "📡 Get current connection details:"  
echo "  sudo wdutil info"
echo ""
echo "🛜 Manual airport scan:"
echo "  /System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s"
echo ""
echo "� Channel analysis:"
echo "  system_profiler SPAirPortDataType | grep -E 'Channel.*\(.*GHz'"