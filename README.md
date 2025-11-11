# Arduino Giga Weather Station

This weather station uses Tomorrow.io APIs to fetch and display real-time weather and forecast data on the Arduino Giga's built-in display.

## Arduino Giga Compatibility Updates

The code has been updated to work with Arduino Giga:

1. **HTTP Client**: Changed from `HTTPClient` (ESP32-specific) to `ArduinoHttpClient` (cross-platform)
2. **WiFi Library**: Uses the Arduino Giga built-in WiFi library
3. **HTTPS Handling**: Uses WiFiClient with port 443 for secure connections
4. **Display**: Uses Arduino_GigaDisplay_GFX library for visual output

## Required Libraries

- ArduinoJson (7.4.2+)
- ArduinoHttpClient (0.6.1+)
- Arduino_GigaDisplay_GFX (1.1.0+)
- AUnit (1.7.1+) - for testing
- WiFi (built-in with Arduino Giga)

## Display Features

The weather station now displays information on the Arduino Giga's built-in TFT display:

- **Color-coded information**: Headers in cyan, errors in red, status in different colors
- **Real-time weather data**: Current conditions displayed prominently with weather icons
- **3-Day forecast**: Condensed forecast view due to screen space
- **Status updates**: WiFi connection status, update timers
- **Auto-refresh**: Screen clears and updates every 5 minutes
- **Weather icons**: Simple geometric icons for visual weather representation

### SVG Support

**Note**: The Arduino_GigaDisplay_GFX library does not support SVG graphics natively. The library is based on Adafruit_GFX which provides basic shapes (circles, rectangles, triangles, lines) and text rendering, but not vector graphics parsing.

As an alternative, the weather station includes a custom `WeatherIcons.h` library that creates simple weather icons using basic geometric shapes:

- ☀️ Sun icon (circle with rays)
- ☁️ Cloud icon (overlapping circles)
- 🌧️ Rain icon (cloud + rain drops)
- 💨 Wind icon (curved lines)
- 🌡️ Thermometer icon
- 💧 Humidity icon (water drop)
- ☀️ UV icon (radiating rays)

## File Structure

```
today/
├── today.ino              # Main Arduino sketch
├── credentials.h          # WiFi credentials and API key
├── today_test.ino.txt     # Unit tests (rename to .ino to run tests)
├── build.sh              # Build script for easy compilation
├── deploy.sh             # Full auto-deployment script
├── deploy-simple.sh      # Simple deployment script (manual port)
├── quick-deploy.sh       # Interactive deployment helper
├── arduino.json          # Arduino CLI project configuration
├── .vscode/               # VS Code configuration for include paths
│   ├── c_cpp_properties.json
│   └── settings.json
└── lib/
    ├── WeatherRealtime.h  # Real-time weather data handling
    ├── WeatherForecast.h  # Forecast data handling
    ├── WeatherIcons.h     # Weather icon drawing functions
    └── Display.h          # Data display functions
```

## Quick Start (Recommended)

1. Copy your WiFi credentials and Tomorrow.io API key to `credentials.h`
2. Connect your Arduino Giga via USB
3. Run the auto-deployment script:
   ```bash
   ./deploy.sh
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
   arduino-cli lib install ArduinoJson ArduinoHttpClient Arduino_GigaDisplay_GFX AUnit
   ```
2. Install Arduino Giga core if not already installed:
   ```bash
   arduino-cli core install arduino:mbed_giga
   ```
3. Compile and upload to Arduino Giga:
   ```bash
   arduino-cli compile --fqbn arduino:mbed_giga:giga .
   arduino-cli upload -p <PORT> --fqbn arduino:mbed_giga:giga .
   ```
   Or use the included build script: `./build.sh`
4. Open Serial Monitor to see weather data

## VS Code IntelliSense Configuration

If you experience include path issues in VS Code, the project includes pre-configured settings:

- **c_cpp_properties.json**: Configures include paths for Arduino Giga libraries
- **settings.json**: Arduino and C++ IntelliSense settings
- **arduino.json**: Arduino CLI project configuration

After opening the project in VS Code, the include statements should resolve automatically.

## API Data Points

### Real-time Weather:

- UV Index
- Humidity (%)
- Wind Speed (km/h)
- Wind Direction (degrees)
- Cloud Cover (%)

### 7-Day Forecast:

- Cloud Cover Average (%)
- Temperature Average (°C)
- Apparent Temperature Average (°C)
- UV Index Average
- Wind Speed Average (km/h)
- Wind Direction Average (degrees)

## Testing

To run tests:

1. Rename `today_test.ino.txt` to `today_test.ino`
2. Upload and run tests via Serial Monitor
3. Rename back to avoid conflicts when running main sketch

## Update Frequency

The sketch fetches new data every 5 minutes to respect API rate limits.
