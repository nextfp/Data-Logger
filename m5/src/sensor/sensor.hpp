int getTemperature(int pin);
int getGearPos(int pin);

class RPM
{
private:
    double rpmCounter = 0;

public:
    void RPMPinCallBack();
    int getRPM(int waitTime);
};