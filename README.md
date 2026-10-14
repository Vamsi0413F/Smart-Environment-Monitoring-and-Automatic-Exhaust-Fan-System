# Smart Exhaust Fan using ESP32

A smart exhaust fan controller built with ESP32 and simulated in Wokwi using PlatformIO.

## Features

- DHT22 temperature and humidity monitoring
- MQ-2 gas/smoke monitoring
- PIR motion detection
- Automatic fan control through a relay
- Manual fan mode using a push button
- 16x2 I2C LCD display
- Separate LEDs for fan, gas, temperature, and motion status
- MQ-2 averaging and deadband filtering
- Real-time operation in Wokwi

## Hardware / Simulation Components

- ESP32 DevKit
- DHT22
- MQ-2 gas sensor
- PIR motion sensor
- Relay module
- 16x2 I2C LCD
- Push button
- Green LED - fan status
- Red LED - gas alert
- Yellow LED - temperature alert
- Blue LED - motion alert

## Pin Configuration

| Component | ESP32 GPIO |
|---|---:|
| DHT22 | GPIO 4 |
| MQ-2 analog output | GPIO 34 |
| Relay | GPIO 26 |
| PIR | GPIO 27 |
| Manual button | GPIO 25 |
| Fan LED | GPIO 14 |
| Gas LED | GPIO 13 |
| Temperature LED | GPIO 12 |
| Motion LED | GPIO 15 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |

## Control Logic

The fan is switched ON automatically when at least one of these conditions is true:

- Temperature is above 35 °C
- Gas level is at or above the configured gas threshold
- Motion is detected

The manual button toggles manual mode. In manual mode, the fan remains ON.

## Gas Sensor Processing

The MQ-2 is sampled 30 times and averaged. Small raw-value changes below the configured deadband are ignored to reduce flickering.

The ESP32 ADC range is mapped from 0-4095 to a simulated gas range of 400-5000 ppm.

## Thresholds

```cpp
TEMP_THRESHOLD = 35.0 °C
GAS_THRESHOLD  = 2900
```

## Project Structure

```text
Smart-Exhaust-Fan-ESP32/
├── src/
│   └── main.cpp
├── include/
├── lib/
├── test/
├── diagram.json
├── platformio.ini
├── wokwi.toml
├── README.md
└── .gitignore
```

## Software

- C++
- Arduino Framework
- PlatformIO
- Wokwi

## Libraries

- Adafruit DHT sensor library
- Adafruit Unified Sensor
- LiquidCrystal_I2C

## Note

`main.cpp` in this repository is reconstructed from the recovered project source. `diagram.json` is recreated from the pin definitions and component configuration in that source, so it should be checked in Wokwi before presentation.

## Author

Veda Vamsi Krishna Raghammudi
