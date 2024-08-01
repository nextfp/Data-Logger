#include <Arduino.h>
#include <M5Unified.h>
#include <SD.h>
#include "sensor/sensor.hpp"
#include "ble/ble.hpp"

#define TEMP_SENSOR_PIN 35
#define GEAR_SENSOR_PIN 36
#define RPM_SENSOR_PIN 26
#define ANGLE_SENSOR_PIN 34

#define WAIT_TIME 200

// put function declarations here:
BLEController *IndiBLEController = NULL;
RPM *rpmController = new RPM;
int loopCount = 0;

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
  pinMode(ANGLE_SENSOR_PIN, ANALOG);
  IndiBLEController = new BLEController;
  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN), []()
                  { rpmController->RPMPinCallBack(); }, RISING);
}

void loop()
{
  delay(WAIT_TIME);
  // put your main code here, to run repeatedly:
  int gear = getGearPos(GEAR_SENSOR_PIN);
  int temperature = getTemperature(TEMP_SENSOR_PIN);
  detachInterrupt(RPM_SENSOR_PIN);
  int rpm = rpmController->getRPM(WAIT_TIME);
  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN), []()
                  { rpmController->RPMPinCallBack(); }, RISING);
  int angle = getAngle(ANGLE_SENSOR_PIN);
  IndiBLEController->sendData(String(gear) + "," + String(rpm) + "," + String(temperature) + "," + String(0));
  M5.Lcd.setCursor(10, 10);
  M5.Display.print(gear);
  M5.Lcd.setCursor(10, 50);
  M5.Display.print(temperature);
  M5.Lcd.setCursor(10, 90);
  M5.Display.print(rpm);
  SD.begin(4, SPI, 15000000);
  const char *path = IndiBLEController->getData().c_str();
  const char *filename = ("/" + std::string(path) + ".csv").c_str();
  File file = SD.open(filename, FILE_APPEND);
  file.println(String(gear) + "," + String(rpm) + "," + String(temperature) + "," + String(angle));
  file.close();
}