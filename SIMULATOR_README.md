# ESP Trappe Interface Simulator

This simulator allows you to test and develop the web interface for the ESP Trappe device locally on your computer without needing the actual hardware.

## Features

- **Full Web Interface**: Serves all HTML pages (index, setup, firmware, help)
- **Mock API Endpoints**: Simulates all the ESP API calls with realistic data
- **Parameter Simulation**: Mock settings for timing, current limits, and control modes
- **Interactive Controls**: Test button presses and form submissions

## Quick Start

### Method 1: Using the Batch File (Recommended)
1. Double-click `simulate.bat` in the project root
2. Open your browser to `http://localhost:8080`
3. The interface will load with mock data

### Method 2: PowerShell Command Line
```powershell
.\simulate_server.ps1
```
Or specify a custom port:
```powershell
.\simulate_server.ps1 -Port 3000
```

### Method 3: Command Line
```cmd
powershell -ExecutionPolicy Bypass -File simulate_server.ps1
```

## What You Can Test

### Main Interface (`/`)
- View module information (version, uptime, build date)
- Configure timing parameters (up/down duration)
- Set current limits for horizontal/vertical motors
- Control manual operations (Open/Close/Stop buttons)
- Switch between Manual (0) and Auto (1) modes
- Save configuration

### Setup Page (`/setup`)
- Configure WiFi settings (SSID, password)
- Set network parameters (IP, router, mDNS)
- Configure module name and IoT keys
- Test form submission

### Mock API Endpoints
- `/jsonParam` - Returns current parameters
- `/jsonSetup` - Returns setup configuration
- `/setParam?key=value` - Mock parameter updates
- `/saveParam` - Mock configuration save
- `/setSetup` - Mock setup updates
- `/reset`, `/restart`, `/firmware` - Mock action pages

## Mock Data

The simulator uses realistic sample data:
- Module: "Trappe Simulator" v3.0
- Network: Connected to "MyWiFi"
- Parameters: 30s up/down, 5A max current, etc.

## Development

To modify the mock data, edit the `$mockConfig` and `$mockParams` variables in `simulate_server.ps1`.

## Requirements

- Windows PowerShell (included with Windows)
- No additional dependencies required

## Troubleshooting

- **Port already in use**: Change the port using `-Port` parameter
- **Execution Policy**: Use `-ExecutionPolicy Bypass` if needed
- **Files not found**: Ensure you're running from the project root directory

## Stopping the Server

Press `Ctrl+C` in the PowerShell window or close the terminal.