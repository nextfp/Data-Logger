#include <Arduino.h>
#include "sensor.hpp"

double fixVoltage(double voltage)
{
    return voltage * 5000 / 1100;
}

int getTemperature(int pin)
{
    double voltage = analogReadMilliVolts(pin) / 1000.0;
    double fixedVoltage = fixVoltage(voltage);
    double temperature = (0.1067 * pow(fixedVoltage, 6)) - (2.6257 * pow(fixedVoltage, 5)) + (22.627 * pow(fixedVoltage, 4)) - (93.172 * pow(fixedVoltage, 3)) + (199.71 * pow(fixedVoltage, 2)) - (241.82 * pow(fixedVoltage, 1)) + 202.85; // 202.82などの数値についてはMoteCの温度センサーに付いてくる資料を見よ
    return temperature;
}

int getGearPos(int pin)
{
    double voltage = analogReadMilliVolts(pin) / 1000.0;
    double fixedVoltage = fixVoltage(voltage);

    switch (static_cast<int>(fixedVoltage * 100))
    {
    case 150 ... 190:
        return 1;
    case 200 ... 240:
        return 2;
    case 250 ... 290:
        return 3;
    case 300 ... 340:
        return 4;
    case 350 ... 370:
        return 5;
    case 380 ... 390:
        return 6;
    case 395 ... 500:
        return 0;
    default:
        return 0;
    }
};

void RPM::RPMPinCallBack()
{
    rpmCounter++;
};
int RPM::getRPM(int waitTime)
{
    double rpm = rpmCounter * 60 / waitTime;
    rpmCounter = 0;
    return rpm;
};