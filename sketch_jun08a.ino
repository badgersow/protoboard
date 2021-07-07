#include "Adafruit_CCS811.h"
#include <LiquidCrystal_I2C.h>
#include "Time.h"

#define LED_R 11
#define LED_G 10
#define LED_B 9

#define CO2_PIN 8
#define CO2_THRESHOLD 1000
#define CO2_PERIOD 5000

Adafruit_CCS811 ccs;
LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lastCo2Poll = 0;

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(CO2_PIN, OUTPUT);

  updateLed(LOW, LOW, LOW);

  Serial.begin(9600);
  while (!Serial)
    ;

  // Wait for the sensor to be ready
  ccs.begin();
  while (!ccs.available())
    ;

      lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
}

void loop() {
  updateRgbLed();
  updateCo2Led();
}

void updateRgbLed() {
  if (Serial.available() > 0) {
    int byte = Serial.read();
    if (byte == 'R') {
      updateLed(HIGH, LOW, LOW);
    } else if (byte == 'G') {
      updateLed(LOW, HIGH, LOW);
    } else if (byte == 'B') {
      updateLed(LOW, LOW, HIGH);
    } else if (byte == '0') {
      updateLed(LOW, LOW, LOW);
    } else {
      // Let's signal an error with yellow color
      Serial.print("Unknown byte received: ");
      Serial.println(byte);
      setErrorLed();
    }
  }
}

void updateCo2Led() {
  unsigned long currentTime = millis();
  if (currentTime < lastCo2Poll || currentTime > lastCo2Poll + CO2_PERIOD) {
    if (ccs.available()) {
      if (!ccs.readData()) {
        uint16_t currentReading = ccs.geteCO2();

        Serial.print("CO2: ");
        Serial.println(ccs.geteCO2());
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CO2: ");
        lcd.print(currentReading);

        if (currentReading >= CO2_THRESHOLD) {
          setCo2Led(HIGH);
        } else {
          setCo2Led(LOW);
        }
      } else {
        Serial.println("Problem with CO2 reading!");
        setErrorLed();
      }
    }
    lastCo2Poll = currentTime;
  }
}

void setCo2Led(int value) {
  digitalWrite(CO2_PIN, value);
}

void setErrorLed() {
  updateLed(HIGH, LOW, HIGH);
}

void updateLed(int r, int g, int b) {
  // Inverting the values because of the common anode configuration
  digitalWrite(LED_R, HIGH - r);
  digitalWrite(LED_G, HIGH - g);
  digitalWrite(LED_B, HIGH - b);
}
