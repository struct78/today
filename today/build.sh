#!/bin/bash

echo "=== Weather Station Arduino Giga Build Script ==="
echo "Checking library paths and compilation..."
echo

# Check if arduino-cli is available
if ! command -v arduino-cli &> /dev/null; then
    echo "Error: arduino-cli not found!"
    exit 1
fi

# Set working directory
cd "/Users/dave/Documents/Github/today/today"

echo "1. Checking installed libraries:"
echo "ArduinoJson: $(find /Users/dave/Documents/Arduino/libraries -name "ArduinoJson" 2>/dev/null)"
echo "ArduinoHttpClient: $(find /Users/dave/Documents/Arduino/libraries -name "ArduinoHttpClient" 2>/dev/null)"
echo "Arduino_GigaDisplay_GFX: $(find /Users/dave/Documents/Arduino/libraries -name "Arduino_GigaDisplay_GFX" 2>/dev/null)"
echo "AUnit: $(find /Users/dave/Documents/Arduino/libraries -name "AUnit" 2>/dev/null)"
echo

echo "2. Checking Arduino Giga core:"
arduino-cli core list | grep giga
echo

echo "3. Compiling weather station..."
arduino-cli compile --fqbn arduino:mbed_giga:giga --verbose .

echo
echo "=== Build Complete ==="