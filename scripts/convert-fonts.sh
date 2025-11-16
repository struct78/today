#!/bin/bash

# Font Conversion Script for Arduino Giga Display
# This script converts TrueType fonts to C arrays for use with Arduino GFX

echo "Font Conversion Guide for Arduino Giga Display"
echo "=============================================="

echo ""
echo "STEP 1: Install fontconvert tool"
echo "git clone https://github.com/adafruit/Adafruit-GFX-Library.git"
echo "cd Adafruit-GFX-Library/fontconvert"
echo "make"

echo ""
echo "STEP 2: Convert font to C array"
echo "Usage: ./fontconvert [font.ttf] [size] [first_char] [last_char] > output.h"
echo ""
echo "Examples:"
echo "./fontconvert helvetica.ttf 18 32 126 > HelveticaNeue18pt.h"
echo "./fontconvert helvetica.ttf 24 32 126 > HelveticaNeue24pt.h"
echo "./fontconvert helvetica.ttf 36 32 126 > HelveticaNeue36pt.h"

echo ""
echo "STEP 3: Include in your project"
echo "#include \"fonts/HelveticaNeue18pt.h\""
echo "display.setFont(&HelveticaNeue18pt7b);"

echo ""
echo "LEGAL ALTERNATIVES to Helvetica Neue:"
echo "======================================"
echo "1. Inter (Google Fonts) - Very similar to Helvetica"
echo "2. Open Sans (Google Fonts) - Clean, modern"
echo "3. Source Sans Pro (Adobe) - Open source"
echo "4. Roboto (Google) - Android system font"
echo "5. Liberation Sans - Metric-compatible with Arial"

echo ""
echo "FREE FONT DOWNLOAD SOURCES:"
echo "1. Google Fonts: https://fonts.google.com"
echo "2. Font Squirrel: https://www.fontsquirrel.com"
echo "3. Adobe Fonts (free with account)"

echo ""
echo "CHARACTER RANGES:"
echo "32-126: Basic ASCII (recommended)"
echo "32-255: Extended ASCII (larger file)"
echo "Custom ranges for specific characters only"

echo ""
echo "FONT SIZES FOR WEATHER DISPLAY:"
echo "12pt - Small text (units, labels)"
echo "18pt - Medium text (titles)"
echo "24pt - Large text (main values)"
echo "36pt - Extra large (primary display)"
