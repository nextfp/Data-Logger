#include <Arduino.h>
#include <M5Unified.h>
#include "sensor/sensor.hpp"
#include "ble/ble.hpp"

#define TEMP_SENSOR_PIN 35
#define GEAR_SENSOR_PIN 36

BLEController *IndiBLEController = NULL;

void setup()
{
  // put your setup code here, to run once:
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextSize(5);
  M5.Lcd.setTextColor(WHITE, BLACK);
  Serial.begin(115200);
  pinMode(TEMP_SENSOR_PIN, ANALOG);
  pinMode(GEAR_SENSOR_PIN, ANALOG);
  IndiBLEController = new BLEController;
}

void loop()
{
  // put your main code here, to run repeatedly:
  int gear = getGearPos(GEAR_SENSOR_PIN);
  int temperature = getTemperature(TEMP_SENSOR_PIN);
  IndiBLEController->sendData(String(gear));
  M5.Lcd.setCursor(10, 10);
  M5.Display.print(gear);
  M5.Lcd.setCursor(10, 50);
  M5.Display.print(temperature);
  delay(100);
}