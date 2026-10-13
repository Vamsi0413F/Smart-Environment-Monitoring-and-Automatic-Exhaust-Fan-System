# Smart Exhaust Fan Using ESP32

An ESP32-based smart ventilation system that monitors temperature, humidity, gas levels, and motion to automate exhaust fan control. The project is developed and simulated using **Wokwi, VS Code, and PlatformIO**, without requiring physical hardware.

## Features

- **Temperature and Humidity Monitoring:** Uses a DHT22 sensor to measure environmental conditions.
- **Gas Detection:** Uses an MQ-2 sensor to monitor gas levels.
- **Automatic Fan Control:** Activates the exhaust fan when predefined temperature, gas, or motion conditions are detected.
- **Motion Detection:** Uses a PIR sensor to detect movement.
- **Manual Control:** A push button allows switching between automatic and manual fan modes.
- **Status Indicators:** LEDs indicate fan operation, gas detection, high temperature, and motion.
- **LCD Display:** A 16×2 I2C LCD displays sensor readings, fan status, and operating mode.
- **Virtual Simulation:** Tests the system using Wokwi without physical components.

## Components Used

| Component | Purpose |
|---|---|
| ESP32 DevKit | Main microcontroller |
| DHT22 | Temperature and humidity measurement |
| MQ-2 | Gas detection |
| PIR sensor | Motion detection |
| Relay module | Fan switching control |
| Push button | Manual mode control |
| 16×2 I2C LCD | Displaying readings and status |
| 4 LEDs | System status indication |
| Breadboard and wires | Virtual circuit connections |

## GPIO Pin Configuration

| Component | ESP32 GPIO |
|---|---:|
| DHT22 data | GPIO 4 |
| MQ-2 analog output | GPIO 34 |
| Relay input | GPIO 26 |
| PIR sensor output | GPIO 27 |
| Manual control button | GPIO 25 |
| Fan status LED (green) | GPIO 14 |
| Gas alert LED (red) | GPIO 13 |
| Temperature alert LED (yellow) | GPIO 12 |
| Motion indicator LED (blue) | GPIO 15 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |

## Working Principle

1. The ESP32 continuously reads temperature and humidity from the DHT22 sensor.
2. The MQ-2 sensor monitors gas levels, while the PIR sensor detects motion.
3. In automatic mode, the fan is activated when the temperature exceeds 35°C, the gas reading reaches the configured threshold, or motion is detected.
4. In manual mode, the fan is forced ON regardless of the automatic control conditions.
5. The LEDs indicate the system status, and the LCD displays the relevant readings and fan state.

**Note:** The MQ-2 threshold is configured using the sensor's raw analog reading. Gas concentration values in the simulation are estimates, not calibrated real-world measurements.

## Software and Tools

- Visual Studio Code
- PlatformIO IDE extension
- Wokwi Simulator
- Arduino framework for ESP32
- C/C++ with Arduino libraries

### Libraries

- DHT sensor library by Adafruit
- Adafruit Unified Sensor
- LiquidCrystal I2C library

## Installation and Setup

1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install the PlatformIO IDE extension.
3. Clone or download this repository:

   ```bash
   git clone https://github.com/YOUR_USERNAME/Smart-Exhaust-Fan-ESP32.git
   cd Smart-Exhaust-Fan-ESP32
   ```

4. Open the project folder in VS Code.
5. Ensure the PlatformIO configuration, firmware source file, Wokwi circuit diagram, and required libraries are present.
6. Build the firmware using the PlatformIO **Build** command.
7. Launch the Wokwi simulation using the project's configured Wokwi workflow.
8. Change sensor values and trigger the motion sensor to test the fan-control behavior.

> **Important:** The firmware source location must match the PlatformIO configuration. If your code is kept in the root-level `sketch.ino` file, ensure your existing configuration supports that layout before building.

## Project Structure

```text
Smart-Exhaust-Fan-ESP32/
├── sketch.ino          # ESP32 firmware (root-level layout)
├── diagram.json        # Wokwi circuit configuration
├── platformio.ini      # PlatformIO configuration
├── wokwi.toml          # Wokwi simulation configuration
├── libraries.txt       # Library list, if used
├── .gitignore
└── README.md
```

The exact files may vary depending on your current project configuration.

## Applications

- Smart home ventilation
- Indoor air-quality monitoring prototypes
- Gas-alert and ventilation systems
- IoT and embedded-systems learning
- Virtual prototyping of automation projects

## Future Improvements

- Add Wi-Fi connectivity and remote monitoring.
- Integrate a mobile dashboard for real-time sensor readings.
- Add data logging and historical analysis.
- Calibrate the gas sensor for more reliable concentration estimates.
- Add notifications for high-temperature and gas-alert conditions.


## License

This project is intended for educational and demonstration purposes. Add an appropriate open-source license if you plan to distribute the source code publicly.
