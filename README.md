# Today

Today is a touch-enabled Arduino display that integrates multiple APIs to show real-time weather data, forecasts, and pool temperature on the Arduino Giga's built-in display with full HTTPS support.

## Features

- 🌤️ **Real-time Weather Data**: Current conditions with custom pixel-art weather icons
- 🏊 **Pool Temperature**: Swimming pool temperature monitoring with custom emoji display  
- 📱 **Touch Interface**: Touch-to-wake display with power management
- 🔐 **Secure HTTPS**: Full SSL/TLS encryption for API communications
- 🎨 **Custom Graphics**: Hand-drawn weather icons with light grey cloud outlines
- 🌈 **Rich Color Palette**: 57 predefined 16-bit RGB colors for beautiful displays
- 📊 **7-Day Forecast**: Extended weather outlook
- ⏰ **NTP Time Sync**: Network Time Protocol for accurate timestamps
- 🔄 **Auto-Updates**: Fresh data every 8 minutes with rate limiting
- 🖥️ **Landscape Display**: Optimized for flat/horizontal viewing
- 🔧 **Modular Architecture**: Clean separation of concerns with reusable components

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

### Multi-API Integration

- **Weather API**: Tomorrow.io for real-time conditions and 7-day forecasts
- **Pool API**: Custom pool temperature monitoring with 🏊 emoji display
- **NTP Protocol**: Network Time Protocol for accurate time synchronization
- **Unified HTTP Client**: Shared HTTPS client with SSL support and manual fallback

### Advanced Time Management

- **NTP Synchronization**: Automatic time sync with network time servers
- **Unix Timestamp Handling**: Precise time calculations and formatting
- **Smart Resync Logic**: Periodic time synchronization to maintain accuracy
- **Time Display**: Human-readable "time ago" formatting (e.g., "2 minutes ago")

### HTTPS Security

- **WiFiSSLClient**: Secure SSL/TLS connections with `connectSSL()` method
- **Certificate Validation**: Automatic certificate chain verification  
- **Encrypted Communication**: All API data fetched over HTTPS (port 443)
- **Manual HTTP Fallback**: Custom HTTP implementation for compatibility issues

### Slideshow System

- **Multi-Screen Display**: Weather, forecast, and pool temperature screens
- **8-Minute Update Cycle**: Coordinated updates respecting API rate limits
- **Touch Navigation**: Touch to cycle through different data displays
- **Power Management**: Touch-to-wake with automatic display sleep

### Touch System

- **Arduino_GigaDisplayTouch**: Hardware touch detection with multi-point support
- **State Management**: Touch press detection (not continuous touch)
- **Debouncing**: 2000ms debounce prevents rapid display cycling
- **Power Integration**: Touch events control display backlight state

### Weather Data Processing

- **JSON Parsing**: ArduinoJson library with 8KB buffer for forecast data
- **Data Structures**: Organized structs for realtime, forecast, and pool data
- **Error Handling**: Comprehensive HTTP status code and parsing error management
- **Memory Management**: Efficient memory usage with static allocations
- **Multi-Source Integration**: Weather, pool, and time data coordination

### File Structure

```
today/
├── README.md                     # Project documentation
├── .github/                      # GitHub configuration and documentation
│   └── copilot-instructions.md   # AI coding guidelines and patterns
├── scripts/                      # Build and deployment automation
│   ├── README.md                 # Deployment documentation  
│   ├── build.sh                  # Build script with library detection
│   ├── deploy.sh                 # Auto-deployment with device detection
│   └── convert-fonts.sh          # Font conversion utilities
├── examples/                     # Sample API responses
│   ├── forecast.json             # Example weather forecast data
│   └── realtime.json             # Example real-time weather data
├── fonts/                        # Font assets and conversion tools
│   ├── Inter.ttc                 # Inter font family
│   ├── ModernFonts.h            # Font definitions
│   └── extras/                   # Additional font formats
└── today/                        # Main Arduino project
    ├── today.ino                 # Main sketch with slideshow logic
    ├── credentials.h             # WiFi/API credentials (Melbourne, Australia)
    ├── credentials.example.h     # Credentials template
    ├── arduino.json              # Arduino CLI configuration
    ├── monitor.sh                # Serial monitoring script
    ├── .vscode/                  # VS Code IntelliSense configuration
    └── lib/                      # Project libraries and components
        ├── Display.h             # Touch-enabled display with 57 colors
        ├── HttpClient.h          # Unified HTTPS client with SSL support
        ├── Logger.h              # Debug logging utilities
        ├── PoolTemperature.h     # Pool API integration with emoji display
        ├── TimeManager.h         # NTP time synchronization and formatting
        ├── WeatherRealtime.h     # Real-time weather API client
        ├── WeatherForecast.h     # 7-day forecast API client
        ├── WeatherIcons.h        # Custom pixel-art weather icons
        └── fonts/                # Converted font headers
            ├── InterRegular12pt.h    # Small text font
            ├── InterMedium24pt.h     # Medium display font
            └── InterBold18pt.h       # Bold accent font
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
✅ **HTTPS Working**: Secure SSL connections to weather and pool APIs  
✅ **Weather Data**: Real-time and 7-day forecast data retrieval  
✅ **Pool Temperature**: Swimming pool monitoring with emoji display
✅ **NTP Time Sync**: Accurate network time synchronization
✅ **Touch Interface**: Responsive touch-to-wake functionality  
✅ **Display Graphics**: Custom weather icons with landscape orientation  
✅ **Auto-Updates**: 8-minute refresh cycle operational with multi-API coordination

**Sketch Size**: ~500KB (25% of Arduino Giga flash memory)  
**RAM Usage**: ~70KB (13% of available memory)

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

### Pool Temperature Data:

- **Real-time Monitoring**: Current pool temperature in Celsius
- **Visual Display**: Swimming emoji (🏊) with temperature reading
- **Time Tracking**: Last update timestamp with NTP synchronization
- **API Integration**: Secure HTTPS connection to pool monitoring service

### Security & Rate Limiting:

- **HTTPS Only**: All API calls use SSL/TLS encryption
- **API Authentication**: Secure API key transmission for weather data
- **Rate Limiting**: 8-minute update intervals (480 seconds) for API protection
- **Error Handling**: Network timeout, retry logic, and connection fallbacks

## Deployment Scripts

- **scripts/deploy.sh**: Auto-detects Arduino, installs libraries, compiles, uploads
- **scripts/build.sh**: Compilation script with comprehensive library detection
- **scripts/convert-fonts.sh**: Font conversion utilities for custom typefaces
- **scripts/README.md**: Detailed deployment documentation
- **today/monitor.sh**: Serial monitor helper script
- **today/arduino.json**: Arduino CLI project configuration

## Development Features

### Code Quality

- **GitHub Copilot Integration**: AI coding guidelines in `.github/copilot-instructions.md`
- **Early Return Patterns**: Clean, readable code with guard clauses
- **Modular Architecture**: Separated concerns with reusable components
- **Comprehensive Logging**: Debug output via `Logger.h` utilities
- **Error Handling**: Robust error management with graceful fallbacks

### Font System

- **Inter Font Family**: Modern, readable typography
- **Multiple Weights**: Regular, Medium, and Bold variants
- **Size Options**: 12pt, 18pt, and 24pt for different UI elements
- **Conversion Tools**: Scripts for generating Arduino-compatible font headers

### Example API Data

The project includes sample API responses in the `examples/` directory:
- **forecast.json**: Example 7-day weather forecast data structure
- **realtime.json**: Example real-time weather data structure

These examples help understand the data format and can be used for offline development and testing.

## Update Frequency

The sketch fetches new data every 8 minutes to respect API rate limits and coordinate multiple data sources efficiently.
