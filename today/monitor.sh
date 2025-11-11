#!/bin/zsh

# monitor.sh - Serial monitor helper for Arduino Giga Weather Station

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo "${BLUE}📺 Arduino Giga Weather Station Serial Monitor${NC}"
echo "=============================================="
echo

# Auto-detect Arduino port
GIGA_PORT=""

if command -v jq &> /dev/null; then
    BOARDS=$(arduino-cli board list --format json 2>/dev/null)
    if [ $? -eq 0 ] && [ -n "$BOARDS" ]; then
        GIGA_PORT=$(echo "$BOARDS" | jq -r '.detected_ports[]? | select(.boards[]?.name? | contains("Giga")) | .port.address' 2>/dev/null | head -1)
    fi
fi

if [ -n "$GIGA_PORT" ]; then
    echo "${GREEN}✅ Found Arduino Giga on port: $GIGA_PORT${NC}"
    echo "${YELLOW}Starting serial monitor (Ctrl+C to exit)...${NC}"
    echo
    sleep 1
    arduino-cli monitor -p "$GIGA_PORT" --config baudrate=115200
else
    echo "${YELLOW}Available ports:${NC}"
    arduino-cli board list | grep -E "^/dev/" | awk '{print "  " $1}'
    echo
    echo -n "Enter the port for your Arduino Giga: "
    read GIGA_PORT
    
    if [ -n "$GIGA_PORT" ]; then
        echo
        echo "${GREEN}Starting serial monitor on $GIGA_PORT (Ctrl+C to exit)...${NC}"
        sleep 1
        arduino-cli monitor -p "$GIGA_PORT" --config baudrate=115200
    else
        echo "No port specified."
        exit 1
    fi
fi