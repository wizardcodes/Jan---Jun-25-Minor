#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include <Adafruit_BMP280.h>
#include <DHT.h>             

// --- Pin Definitions ---
#define DHTPIN 2       // Digital pin connected to the DHT sensor (D2)
#define DHTTYPE DHT11  // Type of DHT sensor
#define MQ135_PIN A0   // Analog pin connected to the MQ-135 sensor (A0)

// --- Sensor Objects ---
// Initialize the I2C LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2); 
Adafruit_BMP280 bmp; // I2C BMP280 sensor
DHT dht(DHTPIN, DHTTYPE); // DHT sensor

void setup() {
  Serial.begin(9600);
  Serial.println("Environmental Sensor Test");

  // Initialize BMP280
  if (!bmp.begin(0x77)) { // Check if BMP280 is found (common I2C address)
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    // Consider adding LCD message here too
    lcd.init(); 
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("BMP280 Error!");
    while (1); // Halt execution
  }
  Serial.println("BMP280 Initialized.");

  // Initialize DHT11
  dht.begin();
  Serial.println("DHT11 Initialized.");
  pinMode(MQ135_PIN, INPUT); // Set MQ135 pin as input
  Serial.println("MQ-135 Pin Initialized.");


  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");
  delay(1000); // Short delay to show message
}

void loop() {
  // --- Read Sensors ---
  float bmp_temp = bmp.readTemperature();
  float bmp_pressure = bmp.readPressure(); // In Pascals
  float dht_humidity = dht.readHumidity();
  float dht_temp = dht.readTemperature(); 
  int mq135_value = analogRead(MQ135_PIN); // Read raw analog value (0-1023)

  // Check if any DHT readings failed and skip display if so.
  if (isnan(dht_humidity)) { // You could also check isnan(dht_temp) if using it
    Serial.println("Failed to read from DHT sensor!");
    // Optionally display error on LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("DHT Error!");
    delay(2000); // Wait before trying again
    return; // Skip the rest of the loop this time
  }

  // --- Display on LCD ---
  lcd.clear(); 

  // Line 1: Temp (BMP) and Humidity (DHT)
  lcd.setCursor(0, 0);
  lcd.print("T:"); 
  lcd.print(bmp_temp, 1); // Temp with 1 decimal place
  //lcd.print((char)223); // Degree symbol (may not work on all LCDs)
  lcd.print("C H:"); 
  lcd.print(dht_humidity, 0); // Humidity with 0 decimal places
  lcd.print("%");  

  // Line 2: Pressure (BMP) and Air Quality (MQ135 raw)
  lcd.setCursor(0,1);
  lcd.print("P:"); 
  lcd.print(bmp_pressure / 100.0F, 0); // Pressure in hPa, 0 decimal places
  lcd.print("hPa AQ:"); 
  lcd.print(mq135_value); // Raw Analog Value

  // --- Print to Serial Monitor ---
  Serial.println("--- Sensor Readings ---");
  Serial.print("BMP Temperature = ");
  Serial.print(bmp_temp);
  Serial.println(" *C");

  Serial.print("DHT Humidity = ");
  Serial.print(dht_humidity);
  Serial.println(" %");

  Serial.print("BMP Pressure = ");
  Serial.print(bmp_pressure / 100.0F); // Convert Pa to hPa
  Serial.println(" hPa");

  Serial.print("MQ-135 Analog Value = ");
  Serial.print(mq135_value);
  Serial.println(" (raw)"); // Indicate it's the raw value
  Serial.println("-----------------------");
  Serial.println(); // Add a blank line for readability

  // --- Wait for next reading ---
  delay(5000); // Refresh every 5 seconds
}