int getTemperature(int pin);
int getGearPos(int pin);
int getAngle(int pin);

class RPM
{
private:
    double rpmCounter = 0;

public:
    void RPMPinCallBack();
    int getRPM(int waitTime);
};

class IMU_6886
{
public:
    enum Ascale
    {
        AFS_2G = 0,
        AFS_4G,
        AFS_8G,
        AFS_16G
    };

    enum Gscale
    {
        GFS_250DPS = 0,
        GFS_500DPS,
        GFS_1000DPS,
        GFS_2000DPS
    };

    float AccelOffset[3] = {0, 0, 0};

public:
    IMU_6886();
    int Init(uint8_t sda, uint8_t scl);
    void getAccelAdc(int16_t *ax, int16_t *ay, int16_t *az);
    void getGyroAdc(int16_t *gx, int16_t *gy, int16_t *gz);
    void getTempAdc(int16_t *t);

    void getAccelData(float *ax, float *ay, float *az);
    void getGyroData(float *gx, float *gy, float *gz);
    void getTempData(float *t);

    void setGyroFsr(Gscale scale);
    void setAccelFsr(Ascale scale);
    void setAccelOffset();

    void getAhrsData(float *pitch, float *roll, float *yaw);
    void setFIFOEnable(bool enableflag);
    uint8_t ReadFIFO();
    void ReadFIFOBuff(uint8_t *DataBuff, uint16_t Length);
    uint16_t ReadFIFOCount();
    void RestFIFO();
    void setGyroOffset(uint16_t x, uint16_t y, uint16_t z);

public:
    float aRes, gRes, imuId;
    Gscale Gyscale;
    Ascale Acscale;

private:
private:
    void I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer);
    void I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer);
    void updateGres();
    void updateAres();
};