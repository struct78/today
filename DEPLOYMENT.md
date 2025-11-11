# Deployment Scripts for Arduino Giga Weather Station

This directory contains several zsh scripts to make deploying and monitoring your weather station easy.

## 🚀 Deployment Scripts

### `deploy.sh` - Full Auto Deployment (Recommended)

- **Auto-detects** Arduino Giga board
- **Automatically installs** missing libraries
- **Compiles and uploads** the sketch
- **Offers to open** Serial Monitor
- **Most comprehensive** option

```bash
./deploy.sh
```

### `deploy-simple.sh` - Simple Deployment

- **Manual port selection** if auto-detection fails
- **Basic compile and upload** functionality
- **Fallback option** for problematic setups

```bash
./deploy-simple.sh
```

### `quick-deploy.sh` - Interactive Helper

- **Shows all deployment options**
- **Interactive menu** for easy selection
- **Great for beginners**

```bash
./quick-deploy.sh
```

## 🔨 Utility Scripts

### `build.sh` - Compile Only

- **Compilation only**, no upload
- **Library path verification**
- **Build testing**

```bash
./build.sh
```

### `monitor.sh` - Serial Monitor

- **Auto-detect board** and open serial monitor
- **Easy debugging** and log viewing
- **115200 baud rate** configured

```bash
./monitor.sh
```

## 📋 Quick Reference

1. **First time setup**: `./deploy.sh`
2. **Re-deploy after changes**: `./deploy.sh`
3. **View debug output**: `./monitor.sh`
4. **Just compile**: `./build.sh`
5. **Need help?**: `./quick-deploy.sh`

## 🔧 Manual Commands

If you prefer manual control:

```bash
# Compile
arduino-cli compile --fqbn arduino:mbed_giga:giga .

# Upload (replace PORT with your actual port)
arduino-cli upload -p /dev/cu.usbmodem1234 --fqbn arduino:mbed_giga:giga .

# Monitor
arduino-cli monitor -p /dev/cu.usbmodem1234 --config baudrate=115200
```

## 📱 Monitoring Your Weather Station

After deployment:

- Weather data appears on the Arduino Giga display
- Debug output visible in Serial Monitor
- Updates every 30 seconds (configurable in code)
- WiFi connection status shown on display

## 🚨 Troubleshooting

1. **Board not detected**: Check USB connection and try `deploy-simple.sh`
2. **Upload fails**: Press reset button on Giga, try different USB port
3. **Libraries missing**: Run `./deploy.sh` to auto-install
4. **Compilation errors**: Check include paths with `./build.sh`

Happy weather monitoring! 🌤️
