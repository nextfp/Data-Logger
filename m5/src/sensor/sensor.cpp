#include <Arduino.h>
#include <Wire.h>
#include "sensor.hpp"
#include <math.h>
#include <M5Stack.h>

#define IMU_6886_ADDRESS 0x68
#define IMU_6886_WHOAMI 0x75
#define IMU_6886_ACCEL_INTEL_CTRL 0x69
#define IMU_6886_SMPLRT_DIV 0x19
#define IMU_6886_INT_PIN_CFG 0x37
#define IMU_6886_INT_ENABLE 0x38
#define IMU_6886_ACCEL_XOUT_H 0x3B
#define IMU_6886_ACCEL_XOUT_L 0x3C
#define IMU_6886_ACCEL_YOUT_H 0x3D
#define IMU_6886_ACCEL_YOUT_L 0x3E
#define IMU_6886_ACCEL_ZOUT_H 0x3F
#define IMU_6886_ACCEL_ZOUT_L 0x40

#define IMU_6886_TEMP_OUT_H 0x41
#define IMU_6886_TEMP_OUT_L 0x42

#define IMU_6886_GYRO_XOUT_H 0x43
#define IMU_6886_GYRO_XOUT_L 0x44
#define IMU_6886_GYRO_YOUT_H 0x45
#define IMU_6886_GYRO_YOUT_L 0x46
#define IMU_6886_GYRO_ZOUT_H 0x47
#define IMU_6886_GYRO_ZOUT_L 0x48

#define IMU_6886_USER_CTRL 0x6A
#define IMU_6886_PWR_MGMT_1 0x6B
#define IMU_6886_PWR_MGMT_2 0x6C
#define IMU_6886_CONFIG 0x1A
#define IMU_6886_GYRO_CONFIG 0x1B
#define IMU_6886_ACCEL_CONFIG 0x1C
#define IMU_6886_ACCEL_CONFIG2 0x1D
#define IMU_6886_FIFO_EN 0x23

#define IMU_6886_FIFO_ENABLE 0x23
#define IMU_6886_FIFO_COUNT 0x72
#define IMU_6886_FIFO_R_W 0x74
#define IMU_6886_GYRO_OFFSET 0x13
// #define G (9.8)
#define RtA 57.324841
#define AtR 0.0174533
#define Gyro_Gr 0.0010653

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

int getAngle(int pin)
{
    int voltage = 0;
    for (int i = 0; i < 10; i++)
    {
        voltage += analogReadMilliVolts(pin);
    }
    voltage /= 10;
    int angle = voltage - 1670;
    return angle;
};

void RPM::RPMPinCallBack()
{
    rpmCounter++;
};
int RPM::getRPM(int waitTime)
{
    double rpm = rpmCounter * 60 * 1000 / waitTime / 8;
    rpmCounter = 0;
    return rpm;
};

IMU_6886::IMU_6886()
{
}

void IMU_6886::I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer)
{
    M5.I2C.readBytes(driver_Addr, start_Addr, number_Bytes, read_Buffer);
}

void IMU_6886::I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer)
{
    M5.I2C.writeBytes(driver_Addr, start_Addr, write_Buffer, number_Bytes);
}

int IMU_6886::Init(uint8_t sda, uint8_t scl)
{
    unsigned char tempdata[1];
    unsigned char regdata;

    Gyscale = GFS_2000DPS;
    Acscale = AFS_8G;

    Wire.begin(sda, scl);

    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_WHOAMI, 1, tempdata);
    imuId = tempdata[0];
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_PWR_MGMT_1, 1, &regdata);
    delay(10);

    regdata = (0x01 << 7);
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_PWR_MGMT_1, 1, &regdata);
    delay(10);

    regdata = (0x01 << 0);
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_PWR_MGMT_1, 1, &regdata);
    delay(10);

    // +- 8g
    regdata = 0x10;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_ACCEL_CONFIG, 1, &regdata);
    delay(1);

    // +- 2000 dps
    regdata = 0x18;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_GYRO_CONFIG, 1, &regdata);
    delay(1);

    // 1khz output
    regdata = 0x01;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_CONFIG, 1, &regdata);
    delay(1);

    // 2 div, FIFO 500hz out
    regdata = 0x01;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_SMPLRT_DIV, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_INT_ENABLE, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_ACCEL_CONFIG2, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_USER_CTRL, 1, &regdata);
    delay(1);

    regdata = 0x00;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_FIFO_EN, 1, &regdata);
    delay(1);

    regdata = 0x22;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_INT_PIN_CFG, 1, &regdata);
    delay(1);

    regdata = 0x01;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_INT_ENABLE, 1, &regdata);

    delay(10);

    setGyroFsr(Gyscale);
    setAccelFsr(Acscale);

    setAccelOffset();
    return 0;
}

void IMU_6886::getAccelAdc(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t buf[6];
    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_ACCEL_XOUT_H, 6, buf);

    *ax = ((int16_t)buf[0] << 8) | buf[1];
    *ay = ((int16_t)buf[2] << 8) | buf[3];
    *az = ((int16_t)buf[4] << 8) | buf[5];
}

void IMU_6886::getGyroAdc(int16_t *gx, int16_t *gy, int16_t *gz)
{
    uint8_t buf[6];
    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_GYRO_XOUT_H, 6, buf);

    *gx = ((uint16_t)buf[0] << 8) | buf[1];
    *gy = ((uint16_t)buf[2] << 8) | buf[3];
    *gz = ((uint16_t)buf[4] << 8) | buf[5];
}

void IMU_6886::getTempAdc(int16_t *t)
{
    uint8_t buf[2];
    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_TEMP_OUT_H, 2, buf);

    *t = ((uint16_t)buf[0] << 8) | buf[1];
}

// Possible gyro scales (and their register bit settings)
void IMU_6886::updateGres()
{
    switch (Gyscale)
    {
    case GFS_250DPS:
        gRes = 250.0 / 32768.0;
        break;
    case GFS_500DPS:
        gRes = 500.0 / 32768.0;
        break;
    case GFS_1000DPS:
        gRes = 1000.0 / 32768.0;
        break;
    case GFS_2000DPS:
        gRes = 2000.0 / 32768.0;
        break;
    }
}

// Possible accelerometer scales (and their register bit settings) are:
// 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
// Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
void IMU_6886::updateAres()
{
    switch (Acscale)
    {
    case AFS_2G:
        aRes = 2.0 / 32768.0;
        break;
    case AFS_4G:
        aRes = 4.0 / 32768.0;
        break;
    case AFS_8G:
        aRes = 8.0 / 32768.0;
        break;
    case AFS_16G:
        aRes = 16.0 / 32768.0;
        break;
    }
}

void IMU_6886::setGyroFsr(Gscale scale)
{
    unsigned char regdata;
    regdata = (scale << 3);
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_GYRO_CONFIG, 1, &regdata);
    delay(10);
    Gyscale = scale;
    updateGres();
}

void IMU_6886::setAccelFsr(Ascale scale)
{
    unsigned char regdata;
    regdata = (scale << 3);
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_ACCEL_CONFIG, 1, &regdata);
    delay(10);
    Acscale = scale;
    updateAres();
}

void IMU_6886::setAccelOffset()
{
    float ax = 0;
    float ay = 0;
    float az = 0;

    getAccelData(&ax, &ay, &az);

    AccelOffset[0] = 0 - ax;
    AccelOffset[1] = 0 - ay;
    AccelOffset[2] = -1 - az;
}

void IMU_6886::getAccelData(float *ax, float *ay, float *az)
{
    int16_t accXArray[5] = {0, 0, 0, 0, 0};
    int16_t accYArray[5] = {0, 0, 0, 0, 0};
    int16_t accZArray[5] = {0, 0, 0, 0, 0};
    for (int i = 0; i < 5; i++)
    {
        getAccelAdc(&accXArray[i], &accYArray[i], &accZArray[i]);
    }
    int16_t accX = (accXArray[0] + accXArray[1] + accXArray[2] + accXArray[3] + accXArray[4]) / 5;
    int16_t accY = (accYArray[0] + accYArray[1] + accYArray[2] + accYArray[3] + accYArray[4]) / 5;
    int16_t accZ = (accZArray[0] + accZArray[1] + accZArray[2] + accZArray[3] + accZArray[4]) / 5;

    *ax = (float)accX * aRes + AccelOffset[0];
    *ay = (float)accY * aRes + AccelOffset[1];
    *az = (float)accZ * aRes + AccelOffset[2];
}

void IMU_6886::getGyroData(float *gx, float *gy, float *gz)
{
    int16_t gyroX = 0;
    int16_t gyroY = 0;
    int16_t gyroZ = 0;
    getGyroAdc(&gyroX, &gyroY, &gyroZ);

    *gx = (float)gyroX * gRes;
    *gy = (float)gyroY * gRes;
    *gz = (float)gyroZ * gRes;
}

void IMU_6886::getTempData(float *t)
{
    int16_t temp = 0;
    getTempAdc(&temp);

    *t = (float)temp / 326.8 + 25.0;
}

void IMU_6886::setGyroOffset(uint16_t x, uint16_t y, uint16_t z)
{
    uint8_t buf_out[6];
    buf_out[0] = x >> 8;
    buf_out[1] = x & 0xff;
    buf_out[2] = y >> 8;
    buf_out[3] = y & 0xff;
    buf_out[4] = z >> 8;
    buf_out[5] = z & 0xff;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_GYRO_OFFSET, 6, buf_out);
}

void IMU_6886::setFIFOEnable(bool enableflag)
{
    uint8_t regdata = 0;
    regdata = enableflag ? 0x18 : 0x00;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_FIFO_ENABLE, 1, &regdata);
    regdata = enableflag ? 0x40 : 0x00;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_USER_CTRL, 1, &regdata);
}

uint8_t IMU_6886::ReadFIFO()
{
    uint8_t ReData = 0;
    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_FIFO_R_W, 1, &ReData);
    return ReData;
}

void IMU_6886::ReadFIFOBuff(uint8_t *DataBuff, uint16_t Length)
{
    uint8_t number = Length / 210;
    for (uint8_t i = 0; i < number; i++)
    {
        I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_FIFO_R_W, 210, &DataBuff[i * 210]);
    }

    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_FIFO_R_W, Length % 210, &DataBuff[number * 210]);
}

void IMU_6886::RestFIFO()
{
    uint8_t buf_out;
    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_USER_CTRL, 1, &buf_out);
    buf_out |= 0x04;
    I2C_Write_NBytes(IMU_6886_ADDRESS, IMU_6886_USER_CTRL, 1, &buf_out);
}

uint16_t IMU_6886::ReadFIFOCount()
{
    uint8_t Buff[2];
    uint16_t ReData = 0;
    I2C_Read_NBytes(IMU_6886_ADDRESS, IMU_6886_FIFO_COUNT, 2, Buff);
    ReData = (Buff[0] << 8) | Buff[1];
    return ReData;
}