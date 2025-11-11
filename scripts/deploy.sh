#!/bin/zsh

# deploy.sh - Today Deployment Script
# This script automatically detects connected Arduino Giga and deploys the project

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project configuration
PROJECT_DIR="$(dirname "$(realpath "$0")")/../today"
BOARD_FQBN="arduino:mbed_giga:giga"
SKETCH_NAME="today"

echo "${BLUE}🚀 Today Deployment${NC}"
echo "================================================="
echo

# Change to project directory
cd "$PROJECT_DIR" || {
    echo "${RED}❌ Error: Could not find project directory: $PROJECT_DIR${NC}"
    exit 1
}

# Check if arduino-cli is installed
if ! command -v arduino-cli &> /dev/null; then
    echo "${RED}❌ Error: arduino-cli not found!${NC}"
    echo "Please install arduino-cli first:"
    echo "brew install arduino-cli"
    exit 1
fi

echo "${YELLOW}🔍 Step 1: Detecting connected Arduino boards...${NC}"
BOARDS=$(arduino-cli board list --format json 2>/dev/null)

if [ $? -ne 0 ] || [ -z "$BOARDS" ]; then
    echo "${RED}❌ Error: Could not detect any boards${NC}"
    echo "Please ensure your Arduino Giga is connected via USB"
    exit 1
fi

# Parse the JSON to find Arduino Giga
GIGA_PORT=$(echo "$BOARDS" | jq -r '.detected_ports[]? | select(.matching_boards[]?.name? | contains("Giga")) | .port.address' 2>/dev/null | head -1)

if [ -z "$GIGA_PORT" ]; then
    echo "${RED}❌ Error: No Arduino Giga detected${NC}"
    echo
    echo "Available boards:"
    echo "$BOARDS" | jq -r '.detected_ports[]? | select(.matching_boards != null) | "  Port: " + .port.address + " - " + .matching_boards[].name' 2>/dev/null || {
        echo "  (Could not parse board information)"
        echo "$BOARDS" | jq -r '.detected_ports[]? | "  Port: " + .port.address + " - " + .port.label' 2>/dev/null || {
            echo "  (No board information available)"
        }
    }
    echo
    echo "Please check:"
    echo "1. Arduino Giga is connected via USB"
    echo "2. Giga is powered on"
    echo "3. USB cable supports data transfer"
    exit 1
fi

echo "${GREEN}✅ Found Arduino Giga on port: $GIGA_PORT${NC}"
echo

echo "${YELLOW}🔧 Step 2: Verifying Arduino Giga core installation...${NC}"
if ! arduino-cli core list | grep -q "arduino:mbed_giga"; then
    echo "${YELLOW}⚠️  Arduino Giga core not found. Installing...${NC}"
    arduino-cli core install arduino:mbed_giga
    if [ $? -eq 0 ]; then
        echo "${GREEN}✅ Arduino Giga core installed successfully${NC}"
    else
        echo "${RED}❌ Failed to install Arduino Giga core${NC}"
        exit 1
    fi
else
    echo "${GREEN}✅ Arduino Giga core is installed${NC}"
fi

echo

echo "${YELLOW}📚 Step 3: Checking required libraries...${NC}"
REQUIRED_LIBS=("ArduinoJson" "ArduinoHttpClient" "Arduino_GigaDisplay_GFX" "AUnit")
MISSING_LIBS=()

for lib in "${REQUIRED_LIBS[@]}"; do
    if arduino-cli lib list | grep -q "^$lib"; then
        echo "${GREEN}✅ $lib${NC}"
    else
        echo "${RED}❌ $lib (missing)${NC}"
        MISSING_LIBS+=("$lib")
    fi
done

if [ ${#MISSING_LIBS[@]} -ne 0 ]; then
    echo
    echo "${YELLOW}⚠️  Installing missing libraries...${NC}"
    for lib in "${MISSING_LIBS[@]}"; do
        echo "Installing $lib..."
        arduino-cli lib install "$lib"
        if [ $? -eq 0 ]; then
            echo "${GREEN}✅ $lib installed successfully${NC}"
        else
            echo "${RED}❌ Failed to install $lib${NC}"
            exit 1
        fi
    done
fi

echo

echo "${YELLOW}🔨 Step 4: Compiling display screen sketch...${NC}"
arduino-cli compile --fqbn "$BOARD_FQBN" .

if [ $? -eq 0 ]; then
    echo "${GREEN}✅ Compilation successful${NC}"
else
    echo "${RED}❌ Compilation failed${NC}"
    exit 1
fi

echo

echo "${YELLOW}📤 Step 5: Uploading to Arduino Giga...${NC}"
echo "Port: $GIGA_PORT"
echo "Board: $BOARD_FQBN"
echo

arduino-cli upload -p "$GIGA_PORT" --fqbn "$BOARD_FQBN" .

if [ $? -eq 0 ]; then
    echo
    echo "${GREEN}🎉 SUCCESS! Display screen deployed successfully!${NC}"
    echo
    echo "📋 Next steps:"
    echo "1. Open Serial Monitor to see debug output:"
    echo "   arduino-cli monitor -p $GIGA_PORT"
    echo "2. Check the display for information"
    echo "3. Touch the screen to toggle display on/off"
    echo
    echo "🖥️  Your Today is now running!"
    
    # Ask if user wants to open serial monitor
    # echo
    # read -q "choice?Open Serial Monitor now? (y/n): "
    # echo
    # if [[ $choice == [Yy] ]]; then
    #     echo "${BLUE}📺 Opening Serial Monitor...${NC}"
    #     echo "Press Ctrl+C to exit the monitor"
    #     sleep 2
        arduino-cli monitor -p "$GIGA_PORT" --config baudrate=115200
    # fi
    
else
    echo "${RED}❌ Upload failed${NC}"
    echo
    echo "Troubleshooting tips:"
    echo "1. Check USB connection"
    echo "2. Try pressing the reset button on the Giga"
    echo "3. Make sure no other applications are using the serial port"
    exit 1
fi