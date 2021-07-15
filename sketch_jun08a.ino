#include <LiquidCrystal_I2C.h>
#include "Time.h"

#define LED_R 11
#define LED_G 10
#define LED_B 9

#define CO2_ANALOG_PIN 0
#define CO2_PIN 8
#define CO2_THRESHOLD 1000
#define CO2_PERIOD 1000

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lastCo2Poll = 0;

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(CO2_PIN, OUTPUT);
  analogReference(DEFAULT);

  updateLed(LOW, LOW, LOW);

  Serial.begin(9600);
  while (!Serial)
    ;

  // Turn on the blacklight and print a message.
  lcd.begin();
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
        int currentReading = readCo2Value();

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CO2: ");
        lcd.print(currentReading);
        lcd.print(" ppm");

        if (currentReading >= CO2_THRESHOLD) {
          setCo2Led(HIGH);
        } else {
          setCo2Led(LOW);
        }

    lastCo2Poll = currentTime;
  }
}

int readCo2Value() {
  //Read voltage
  int sensorValue = analogRead(CO2_ANALOG_PIN);

  // The analog signal is converted to a voltage
  float voltage = sensorValue * (5000 / 1024.0);
  
  if(voltage < 400)
  {
    return -1; // Error value
  }
  else
  {
    int voltage_diference=voltage - 400;
    float concentration=voltage_diference * 50.0 / 16.0;
    return (int) concentration;
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
