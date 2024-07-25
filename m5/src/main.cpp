#include <Arduino.h>
#include <M5Unified.h>
#include "sensor/sensor.hpp"
#include "ble/ble.hpp"

#define TEMP_SENSOR_PIN 35
#define GEAR_SENSOR_PIN 36
#define RPM_SENSOR_PIN 26

// put function declarations here:
BLEController *IndiBLEController = NULL;

void addRpmCounter();
int calcRPM();

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
  delay(200);
  detachInterrupt(RPM_SENSOR_PIN);
  // put your main code here, to run repeatedly:
  int gear = getGearPos(GEAR_SENSOR_PIN);
  int temperature = getTemperature(TEMP_SENSOR_PIN);
  int rpm = calcRPM();
  IndiBLEController->sendData(String(gear) + "," + String(rpm) + ",");
  M5.Lcd.setCursor(10, 10);
  M5.Display.print(gear);
  M5.Lcd.setCursor(10, 50);
  M5.Display.print(temperature);
  M5.Lcd.setCursor(10, 90);
  M5.Display.print(rpm);
  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN), addRpmCounter, RISING);
}
double rpmCounter = 0;

void addRpmCounter()
{
  rpmCounter++;
}

int calcRPM()
{
  double rpm = rpmCounter / 100 * 60000;
  rpmCounter = 0;
  return rpm;
}