# GitHub Copilot Instructions for Arduino Weather Display Project

## Project Overview

This is an Arduino Giga R1 WiFi weather display project that fetches data from Tomorrow.io API and displays it on a touch screen. The project emphasizes clean code, proper error handling, and test-driven development.

## Code Style and Patterns

### 1. Function Design

- **Use early return patterns (guard clauses)** instead of nested if/else
- **Single Responsibility Principle**: Each function should have one clear purpose
- **Fail fast**: Handle error conditions early and return immediately
- **Meaningful names**: Functions should clearly describe what they do

```cpp
// ✅ Good - Early return pattern
void fetchWeatherData() {
  if (!isConnected()) {
    displayError("No connection");
    return;
  }

  // Main logic flows linearly
  auto data = fetchFromAPI();
  displayData(data);
}

// ❌ Bad - Nested if/else
void fetchWeatherData() {
  if (isConnected()) {
    auto data = fetchFromAPI();
    displayData(data);
  } else {
    displayError("No connection");
  }
}
```

### 2. Error Handling

- Always check for null pointers before dereferencing
- Validate data before using it
- Provide meaningful error messages
- Use return values to indicate success/failure

### 3. Hardware Abstraction

- Keep hardware-specific code in dedicated files (e.g., `Display.h`, `WeatherRealtime.h`)
- Use clear interfaces between hardware and business logic
- Abstract complex operations into meaningful function names

## Project Structure

### Core Files

- `today.ino` - Main application logic with clean function separation
- `lib/Display.h` - Display control and touch interface
- `lib/WeatherRealtime.h` - Real-time weather API client
- `lib/WeatherForecast.h` - Forecast weather API client
- `credentials.h` - WiFi and API credentials (not in repo)

### Key Constants

- `updateInterval` - 8 minutes (480,000ms) for rate limiting
- `offlineMode` - Boolean flag for test mode vs live API
- Color definitions use 16-bit RGB565 format

### Test Mode

- Always provide test mode functionality with embedded JSON data
- Test mode should mirror live mode behavior exactly
- Use meaningful test data that represents real scenarios

## API Integration

### Rate Limiting

- Tomorrow.io has 25 requests/hour limit
- Use 8-minute intervals for safety (allows 2 calls per update)
- Always check connection status before API calls

### Data Structures

- Use validated data structures (isValid flag)
- Handle parsing failures gracefully
- Provide default values for missing data

## Display Management

### Touch Interface

- Implement touch debouncing
- Provide clear visual feedback
- Support touch-to-wake functionality

### Screen Layout

- Use landscape orientation
- Clear screen before major updates
- Handle different weather conditions with appropriate icons

## Development Workflow

### Scripts Organization

- Keep all scripts in `/scripts` folder
- Use relative paths from project root
- Provide both build and deploy scripts

### VS Code Configuration

- Configure C++ extension for Arduino development
- Set proper include paths for libraries
- Use consistent formatting (K&R brace style)

## Code Generation Guidelines

When generating code for this project:

1. **Always use early return patterns** - avoid nested if/else structures
2. **Include proper error handling** - check for failures and handle gracefully
3. **Use const correctness** - mark variables const when they don't change
4. **Prefer meaningful variable names** - avoid abbreviations
5. **Include debug output** - use Serial.println for important events
6. **Validate inputs** - check parameters before using them
7. **Handle test mode** - support both offline and online modes

## Common Patterns

### Initialization Functions

```cpp
void initializeComponent() {
  Serial.println("Initializing component...");

  if (!component.begin()) {
    Serial.println("Component initialization failed");
    displayError("Component failed to start");
    return;
  }

  Serial.println("Component initialized successfully");
}
```

### Data Fetching Functions

```cpp
DataType fetchData() {
  DataType data = getDefaultData();

  if (offlineMode) {
    Serial.println("Using test data...");
    return loadTestData();
  }

  if (!isConnected()) {
    Serial.println("No connection available");
    return data;
  }

  // Fetch real data
  return fetchFromAPI();
}
```

### Display Functions

```cpp
void displayData(const DataType& data) {
  if (!data.isValid) {
    displayError("Invalid data");
    return;
  }

  clearScreen();
  renderData(data);
}
```

## Testing Strategy

- Always provide test mode with embedded data
- Test data should be realistic and cover edge cases
- Include both success and failure scenarios
- Test touch interface thoroughly

## Hardware Considerations

- Arduino Giga R1 WiFi with 2MB flash, 512KB RAM
- Display Shield with touch interface
- WiFi with SSL/TLS support for API calls
- Power management for display backlight

## Security

- Never commit credentials to repository
- Use credentials.example.h as template
- Validate all external data before processing
- Use HTTPS for all API communication

---

_This file should be updated as the project evolves and new patterns are established._
