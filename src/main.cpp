#include <Arduino.h>                 // Arduino functions
#include <DHT.h>                     // DHT22 sensor library
#include <Wire.h>                    // I2C communication
#include <LiquidCrystal_I2C.h>       // I2C LCD library

// ---------- PIN DEFINITIONS ----------

#define DHT_PIN 4                    // DHT22 data pin
#define MQ2_PIN 34                   // MQ2 gas sensor analog pin
#define RELAY_PIN 26                 // Relay controls the fan
#define PIR_PIN 27                   // PIR detects motion
#define MANUAL_BUTTON 25             // Button for manual mode

#define FAN_LED_PIN 14               // Green LED: fan status
#define GAS_LED_PIN 13               // Red LED: gas alert
#define TEMP_LED_PIN 12              // Yellow LED: temperature alert
#define MOTION_LED_PIN 15            // Blue LED: motion detected

// ---------- DHT22 SENSOR ----------

#define DHT_TYPE DHT22               // Select DHT22 sensor
DHT dht(DHT_PIN, DHT_TYPE);           // Create DHT22 sensor object

// ---------- LCD ----------

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD at I2C address 0x27

// ---------- THRESHOLDS ----------

const float TEMP_THRESHOLD = 35.0;   // Fan turns ON above 35°C
const int GAS_THRESHOLD = 2900;      // Fan turns ON at 2900 ppm gas

// ---------- MQ2 GAS SENSOR ----------

const int GAS_MIN_PPM = 400;         // Minimum simulated gas value
const int GAS_MAX_PPM = 5000;        // Maximum simulated gas value

const int ADC_MIN = 0;               // ESP32 minimum ADC value
const int ADC_MAX = 4095;             // ESP32 maximum ADC value

// ---------- GAS FILTER ----------

const int GAS_RAW_DEADBAND = 80;     // Ignore small sensor fluctuations
const int GAS_SAMPLES = 30;          // Number of readings to average

// ---------- TIMING ----------

const unsigned long SENSOR_INTERVAL = 1000;  // Read sensors every 1 second
const unsigned long BUTTON_DEBOUNCE = 50;   // Button debounce time

// ---------- SENSOR VALUES ----------

float temperature = 28.0;            // Current temperature
float humidity = 65.0;               // Current humidity

int gasRaw = 0;                      // Current raw MQ2 value
int stableGasRaw = 0;                // Stable MQ2 value
int gasPpm = GAS_MIN_PPM;            // Current gas level in ppm

// ---------- SYSTEM STATUS ----------

bool temperatureAlert = false;       // Temperature threshold status
bool gasAlert = false;               // Gas threshold status
bool motionDetected = false;         // PIR motion status

bool manualMode = false;             // Manual mode status
bool fanState = false;               // Fan ON/OFF status

// ---------- BUTTON STATUS ----------

bool lastButtonReading = HIGH;       // Previous button reading
bool stableButtonState = HIGH;       // Stable button state

unsigned long lastButtonChange = 0;  // Last button change time
unsigned long lastSensorRead = 0;    // Last sensor reading time

// ---------- MQ2 GAS SENSOR ----------

int readGasSensor()
{
    long total = 0;                  // Stores total of all readings

    for (int i = 0; i < GAS_SAMPLES; i++)  // Take 30 readings
    {
        total += analogRead(MQ2_PIN);      // Read MQ2 analog value
        delay(2);                          // Small delay between readings
    }

    return total / GAS_SAMPLES;       // Return average reading
}

// ---------- CONVERT ADC TO PPM ----------

int rawToPPM(int raw)
{
    raw = constrain(raw, ADC_MIN, ADC_MAX); // Keep ADC within valid range

    long ppm = map(                       // Convert ADC value to PPM
        raw,
        ADC_MIN,
        ADC_MAX,
        GAS_MIN_PPM,
        GAS_MAX_PPM
    );

    return constrain(                     // Keep PPM within set range
        ppm,
        GAS_MIN_PPM,
        GAS_MAX_PPM
    );
}

// ---------- UPDATE GAS SENSOR ----------

void updateGas()
{
    int newRaw = readGasSensor();          // Get new MQ2 reading

    gasRaw = newRaw;                      // Store raw sensor value

    if (stableGasRaw == 0)                // Check for first reading
    {
        stableGasRaw = newRaw;            // Store first stable value
        gasPpm = rawToPPM(stableGasRaw);  // Convert to PPM
        return;                            // Exit function
    }

    int difference = abs(newRaw - stableGasRaw); // Calculate change

    if (difference < GAS_RAW_DEADBAND)    // Ignore small fluctuations
    {
        return;                           // Keep previous value
    }

    stableGasRaw = newRaw;                // Store new stable value
    gasPpm = rawToPPM(stableGasRaw);      // Convert new value to PPM
}

// ---------- GAS ALERT ----------

void updateGasAlert()
{
    gasAlert = gasPpm >= GAS_THRESHOLD;   // Gas alert above threshold
}

// ---------- FAN CONTROL ----------

void setFan(bool state)
{
    fanState = state;                     // Store fan state

    digitalWrite(                          // Control relay
        RELAY_PIN,
        state ? HIGH : LOW
    );

    digitalWrite(                          // Control green fan LED
        FAN_LED_PIN,
        state ? HIGH : LOW
    );
}

// ---------- AUTOMATIC FAN DECISION ----------

bool automaticFanRequest()
{
    return (
        temperatureAlert ||               // High temperature
        gasAlert ||                       // High gas level
        motionDetected                    // Motion detected
    );
}

// ---------- LED INDICATORS ----------

void updateIndicators()
{
    digitalWrite(                          // Red LED for gas
        GAS_LED_PIN,
        gasAlert ? HIGH : LOW
    );

    digitalWrite(                          // Yellow LED for temperature
        TEMP_LED_PIN,
        temperatureAlert ? HIGH : LOW
    );

    digitalWrite(                          // Blue LED for motion
        MOTION_LED_PIN,
        motionDetected ? HIGH : LOW
    );

    digitalWrite(                          // Green LED for fan
        FAN_LED_PIN,
        fanState ? HIGH : LOW
    );
}

// ---------- LCD DISPLAY ----------

void updateLCD()
{
    lcd.setCursor(0, 0);                  // Move to first line
    lcd.print("                ");        // Clear first line

    lcd.setCursor(0, 0);                  // Start first line

    lcd.print("T:");                      // Display temperature label
    lcd.print(temperature, 1);            // Display temperature
    lcd.print("C G:");                    // Display gas label
    lcd.print(gasPpm);                    // Display gas level

    lcd.setCursor(0, 1);                  // Move to second line
    lcd.print("                ");        // Clear second line

    lcd.setCursor(0, 1);                  // Start second line

    lcd.print("FAN:");                    // Display fan label

    if (fanState)                         // Check fan state
    {
        lcd.print("ON ");                 // Display FAN ON
    }
    else
    {
        lcd.print("OFF");                // Display FAN OFF
    }

    lcd.print(" ");                       // Add space

    if (manualMode)                       // Check manual mode
    {
        lcd.print("MANUAL");              // Display manual mode
    }
    else if (gasAlert)                    // Check gas alert
    {
        lcd.print("GAS!");                // Display gas warning
    }
    else if (temperatureAlert)            // Check temperature alert
    {
        lcd.print("TEMP!");               // Display temperature warning
    }
    else if (motionDetected)              // Check motion
    {
        lcd.print("MOTION");              // Display motion status
    }
    else
    {
        lcd.print("AUTO");                // Display automatic mode
    }
}

// ---------- MANUAL BUTTON ----------

void checkManualButton()
{
    bool reading = digitalRead(MANUAL_BUTTON); // Read button

    if (reading != lastButtonReading)         // Detect button change
    {
        lastButtonChange = millis();          // Record change time
        lastButtonReading = reading;          // Save current reading
    }

    if (millis() - lastButtonChange >= BUTTON_DEBOUNCE) // Debounce
    {
        if (reading != stableButtonState)     // Check stable change
        {
            stableButtonState = reading;     // Update stable state

            if (stableButtonState == LOW)     // Button is pressed
            {
                manualMode = !manualMode;    // Toggle manual mode

                if (manualMode)              // Manual mode ON
                {
                    setFan(true);            // Force fan ON
                }
                else
                {
                    setFan(automaticFanRequest()); // Return to auto
                }

                updateIndicators();          // Update LEDs
                updateLCD();                 // Update LCD
            }
        }
    }
}

// ---------- READ ALL SENSORS ----------

void readSensors()
{
    float newTemperature = dht.readTemperature(); // Read temperature
    float newHumidity = dht.readHumidity();       // Read humidity

    if (!isnan(newTemperature)                     // Check valid temperature
    )
    {
        temperature = newTemperature;              // Update temperature
    }

    if (!isnan(newHumidity))                       // Check valid humidity
    {
        humidity = newHumidity;                    // Update humidity
    }

    updateGas();                                   // Read MQ2 sensor
    updateGasAlert();                              // Check gas threshold

    temperatureAlert = temperature > TEMP_THRESHOLD; // Check temperature

    motionDetected = digitalRead(PIR_PIN) == HIGH;   // Read PIR sensor
}

// ---------- FAN CONTROL ----------

void controlFan()
{
    if (manualMode)                                // Manual mode active
    {
        setFan(true);                              // Keep fan ON
    }
    else
    {
        setFan(automaticFanRequest());             // Use sensor conditions
    }
}

// ---------- SETUP ----------

void setup()
{
    dht.begin();                                   // Start DHT22 sensor

    Wire.begin(21, 22);                            // Start I2C: SDA 21, SCL 22

    lcd.init();                                    // Initialize LCD
    lcd.backlight();                               // Turn LCD backlight ON
    lcd.clear();                                   // Clear LCD

    lcd.setCursor(0, 0);                           // First line
    lcd.print("Smart Exhaust");                   // Startup message

    lcd.setCursor(0, 1);                           // Second line
    lcd.print("Starting...");                     // Startup message

    delay(1500);                                   // Wait for startup

    pinMode(MQ2_PIN, INPUT);                       // MQ2 as input
    pinMode(PIR_PIN, INPUT);                       // PIR as input
    pinMode(MANUAL_BUTTON, INPUT_PULLUP);         // Button as input

    pinMode(RELAY_PIN, OUTPUT);                    // Relay as output
    pinMode(FAN_LED_PIN, OUTPUT);                  // Fan LED as output
    pinMode(GAS_LED_PIN, OUTPUT);                  // Gas LED as output
    pinMode(TEMP_LED_PIN, OUTPUT);                 // Temperature LED
    pinMode(MOTION_LED_PIN, OUTPUT);               // Motion LED

    digitalWrite(RELAY_PIN, LOW);                  // Fan initially OFF
    digitalWrite(FAN_LED_PIN, LOW);                // Green LED OFF
    digitalWrite(GAS_LED_PIN, LOW);                // Red LED OFF
    digitalWrite(TEMP_LED_PIN, LOW);               // Yellow LED OFF
    digitalWrite(MOTION_LED_PIN, LOW);             // Blue LED OFF

    lcd.clear();                                   // Clear LCD

    lcd.setCursor(0, 0);                           // First line
    lcd.print("Reading Sensors");                 // Sensor initialization

    lcd.setCursor(0, 1);                           // Second line
    lcd.print("Please wait...");                  // Waiting message

    delay(1000);                                   // Wait for sensors

    readSensors();                                 // Read all sensors
    controlFan();                                  // Decide fan state
    updateIndicators();                            // Update LEDs
    updateLCD();                                   // Display values
}

// ---------- MAIN LOOP ----------

void loop()
{
    checkManualButton();                           // Check manual button

    if (millis() - lastSensorRead >= SENSOR_INTERVAL) // Every 1 second
    {
        lastSensorRead = millis();                 // Save current time

        readSensors();                             // Read sensors
        controlFan();                              // Control fan
        updateIndicators();                        // Update LEDs
        updateLCD();                               // Update LCD
    }
}