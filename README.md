# Today

Today is a touch-enabled Arduino display that uses Tomorrow.io APIs to fetch and display real-time weather and forecast data on the Arduino Giga's built-in display with full HTTPS support.

## Features

- 🌤️ **Real-time Weather Data**: Current conditions with custom pixel-art weather icons
- 📱 **Touch Interface**: Touch-to-wake display with power management
- 🔐 **Secure HTTPS**: Full SSL/TLS encryption for API communications
- 🎨 **Custom Graphics**: Hand-drawn weather icons with light grey cloud outlines
- 🌈 **Rich Color Palette**: 57 predefined 16-bit RGB colors for beautiful displays
- 📊 **7-Day Forecast**: Extended weather outlook
- 🔄 **Auto-Updates**: Fresh data every 5 minutes
- 🖥️ **Landscape Display**: Optimized for flat/horizontal viewing

## Hardware Requirements

- **Arduino Giga R1 WiFi** with WiFi antenna attached
- **Arduino Giga Display Shield** (built-in TFT with touch)
- WiFi network (WPA2)

## Required Libraries

- **ArduinoJson** (7.4.2+) - JSON parsing for weather data
- **ArduinoHttpClient** (0.6.1+) - HTTP/HTTPS client for API requests
- **Arduino_GigaDisplay_GFX** (1.1.0+) - Graphics and display control
- **Arduino_GigaDisplay** (1.0.2+) - Advanced display features and backlight control
- **Arduino_GigaDisplayTouch** (1.0.1+) - Touch screen interface
- **ArduinoGraphics** (1.1.4+) - Enhanced graphics capabilities
- **lvgl** (9.4.0+) - GUI framework (dependency)
- **AUnit** (1.7.1+) - Unit testing framework
- **WiFi** (built-in) - WiFi connectivity with SSL support

## Display Features

The display features a sophisticated touch-enabled interface system:

### Visual Design

- **Landscape Orientation**: Optimized for horizontal/flat viewing with 90° rotation
- **Custom Weather Icons**: Hand-drawn pixel-art sun and cloud graphics with light grey outlines
- **Rich Color Palette**: 57 predefined colors including weather-specific colors:
  - `MIDNIGHT_BLUE` - Night sky backgrounds
  - `DEEP_SKY_BLUE` - Clear day sky
  - `SUN_YELLOW` - Bright sunshine
  - `CLOUD_GRAY` - Cloud representation
  - `WARM_ORANGE` - Sunset/sunrise colors

### Touch Interface

- **Touch-to-Wake**: Display starts dark, touch anywhere to activate
- **Power Management**: Automatic display sleep for battery conservation
- **Debounced Input**: 2-second debounce prevents accidental rapid toggling
- **Coordinate Logging**: Touch points logged via Serial for debugging

### Display Control

- **Hardware Backlight Control**: True display power management using GigaDisplayBacklight
- **Landscape Mode**: `display.setRotation(1)` for proper orientation
- **High Contrast**: Deep black backgrounds with bright, colorful elements
- **Anti-Flicker**: Proper screen clearing and refresh timing

### Weather Data Display

- **Real-time Conditions**: Current temperature, humidity, wind, UV index
- **7-Day Forecast**: Extended outlook with daily summaries
- **Error Handling**: Clear error messages for network/API issues
- **Auto-Refresh**: Updates every 5 minutes with fresh data

## Technical Architecture

### HTTPS Security

- **WiFiSSLClient**: Secure SSL/TLS connections to Tomorrow.io API
- **Certificate Validation**: Automatic certificate chain verification
- **Encrypted Communication**: All weather data fetched over HTTPS (port 443)

### Touch System

- **Arduino_GigaDisplayTouch**: Hardware touch detection with multi-point support
- **State Management**: Touch press detection (not continuous touch)
- **Debouncing**: 2000ms debounce prevents rapid display cycling
- **Power Integration**: Touch events control display backlight state

### Weather Data Processing

- **JSON Parsing**: ArduinoJson library with 8KB buffer for forecast data
- **Data Structures**: Organized structs for realtime and forecast data
- **Error Handling**: Comprehensive HTTP status code and parsing error management
- **Memory Management**: Efficient memory usage with static allocations

### File Structure

```
today/
├── README.md              # This file
├── scripts/               # Deployment and build scripts
│   ├── README.md         # Deployment documentation
│   ├── deploy.sh         # Auto-deployment with library detection
│   └── build.sh          # Build script
├── today/                # Arduino project folder
│   ├── today.ino         # Main sketch with WiFi and touch handling
│   ├── credentials.h     # WiFi/API credentials (Melbourne, Australia location)
│   ├── today_test.ino.txt # Unit tests
│   ├── arduino.json      # Arduino CLI configuration
│   ├── .vscode/          # VS Code IntelliSense setup
│   └── lib/
│       ├── WeatherRealtime.h  # HTTPS realtime weather API client
│       ├── WeatherForecast.h  # HTTPS forecast API client
│       ├── WeatherIcons.h     # Custom pixel-art weather icons
│       └── Display.h          # Touch-enabled display with 57 colors
```

## Quick Start (Recommended)

1. Copy your WiFi credentials and Tomorrow.io API key to `today/credentials.h`
2. Connect your Arduino Giga via USB
3. Run the auto-deployment script:
   ```bash
   ./scripts/deploy.sh
   ```
   This script will:
   - Auto-detect your Arduino Giga
   - Install missing libraries
   - Compile the sketch
   - Upload to your board
   - Optionally open Serial Monitor

## Alternative Deployment Methods

### Simple Deployment (Manual Port)

```bash
./deploy-simple.sh
```

For cases where auto-detection doesn't work.

### Interactive Helper

```bash
./quick-deploy.sh
```

Shows all available options and guides you through deployment.

### Manual Setup Instructions

1. Install required libraries using Arduino CLI:
   ```bash
   arduino-cli lib install ArduinoJson ArduinoHttpClient Arduino_GigaDisplay_GFX Arduino_GigaDisplay Arduino_GigaDisplayTouch AUnit
   ```
2. Install Arduino Giga core if not already installed:
   ```bash
   arduino-cli core install arduino:mbed_giga
   ```
3. Connect WiFi antenna to Arduino Giga (required for WiFi functionality)
4. Update `credentials.h` with your WiFi network and Tomorrow.io API key
5. Compile and upload:
   ```bash
   arduino-cli compile --fqbn arduino:mbed_giga:giga .
   arduino-cli upload -p <PORT> --fqbn arduino:mbed_giga:giga .
   ```

## Current Status

✅ **WiFi Connected**: Successfully connects with strong signal strength  
✅ **HTTPS Working**: Secure SSL connections to Tomorrow.io API  
✅ **Weather Data**: Real-time and 7-day forecast data retrieval  
✅ **Touch Interface**: Responsive touch-to-wake functionality  
✅ **Display Graphics**: Custom weather icons with landscape orientation  
✅ **Auto-Updates**: 5-minute refresh cycle operational

**Sketch Size**: ~468KB (23% of Arduino Giga flash memory)  
**RAM Usage**: ~69KB (13% of available memory)

## VS Code IntelliSense Configuration

If you experience include path issues in VS Code, the project includes pre-configured settings:

- **c_cpp_properties.json**: Configures include paths for Arduino Giga libraries
- **settings.json**: Arduino and C++ IntelliSense settings
- **arduino.json**: Arduino CLI project configuration

After opening the project in VS Code, the include statements should resolve automatically.

## API Data Points

### Real-time Weather (HTTPS):

- **Environmental**: UV Index, Humidity (%), Cloud Cover (%)
- **Wind**: Speed (km/h), Direction (degrees)
- **Temperature**: Current conditions with apparent temperature
- **Atmospheric**: Pressure, visibility, weather codes

### 7-Day Forecast (HTTPS):

- **Daily Averages**: Temperature, apparent temperature, cloud cover
- **Weather Metrics**: UV index, wind speed/direction, humidity
- **Precipitation**: Rain probability, accumulation, intensity
- **Time Data**: Sunrise/sunset times, moon phases
- **Extremes**: Daily min/max values for all metrics

### Security & Rate Limiting:

- **HTTPS Only**: All API calls use SSL/TLS encryption
- **API Authentication**: Secure API key transmission
- **Rate Limiting**: 5-minute update intervals (300 seconds)
- **Error Handling**: Network timeout and retry logic

## Deployment Scripts

- **scripts/deploy.sh**: Auto-detects Arduino, installs libraries, compiles, uploads
- **scripts/build.sh**: Simple compilation script
- **scripts/README.md**: Detailed deployment documentation
- **today/arduino.json**: Arduino CLI project configuration## Testing

To run tests:

1. Rename `today_test.ino.txt` to `today_test.ino`
2. Upload and run tests via Serial Monitor
3. Rename back to avoid conflicts when running main sketch

## Update Frequency

The sketch fetches new data every 5 minutes to respect API rate limits.
