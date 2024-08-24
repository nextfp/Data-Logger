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
IMU_6886 imuController;

float accX = 0;
float accY = 0;
float accZ = 0;

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
  imuController.Init(32, 33);
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
  imuController.getAccelData(&accX, &accY, &accZ);
  IndiBLEController->sendData(String(gear) + "," + String(rpm) + "," + String(temperature) + "," + String(angle) + "," + String(accX) + "," + String(accY) + "," + String(accZ));
  M5.Lcd.setCursor(10, 10);
  M5.Display.print(gear);
  M5.Lcd.setCursor(10, 50);
  M5.Display.print(temperature);
  M5.Lcd.setCursor(10, 90);
  M5.Display.print(rpm);
  M5.Lcd.setCursor(10, 130);
  M5.Lcd.printf("%.2f   %.2f   %.2f   ", accX, accY, accZ);
  SD.begin(4, SPI, 15000000);
  const char *filename = ("/" + getFilePath() + ".csv").c_str();
  Serial.println(filename);
  File file = SD.open(filename, FILE_APPEND);
  file.println((getTime() + "," + std::to_string(gear) + "," + std::to_string(rpm) + "," + std::to_string(temperature) + "," + std::to_string(angle) + "," + getLatitude() + "," + getLongitude() + "," + getSpeed() + "," + std::to_string(accX) + "," + std::to_string(accY) + "," + std::to_string(accZ)).c_str());
  file.close();
}