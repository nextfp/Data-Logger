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