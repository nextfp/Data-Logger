#include <Arduino.h>
#include <BLEServer.h>

class BLEStatus;

class BLEController
{
private:
    BLEServer *pServer = NULL;
    BLEStatus *status = NULL;
    BLECharacteristic *pTxCharacteristic;
    BLECharacteristic *pRxCharacteristic;
    bool deviceConnected = false;

public:
    BLEController();
    void sendData(String data);
};
