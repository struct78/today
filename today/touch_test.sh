#!/bin/bash

# Touch Test Script for Arduino Giga Weather Station

echo "🖱️  Touch Test Script"
echo "====================="
echo "This script will test the touch-to-wake functionality"
echo ""

# Check if Arduino is connected
ARDUINO_PORT="/dev/cu.usbmodem1101"

if [ ! -e "$ARDUINO_PORT" ]; then
    echo "❌ Arduino not found on $ARDUINO_PORT"
    echo "Available ports:"
    ls /dev/cu.usb* 2>/dev/null || echo "No USB devices found"
    exit 1
fi

echo "✅ Arduino found on $ARDUINO_PORT"
echo ""
echo "📺 Starting touch test..."
echo "Commands you can try:"
echo "  touch - Toggle display on/off"
echo "  help  - Show help"
echo ""
echo "Type 'exit' to quit"
echo ""

# Interactive session
arduino-cli monitor -p $ARDUINO_PORT