#include <Arduino.h>
#include <BLEServer.h>

class BLEStatus;

class RxCallbacks;

class BLEController
{
private:
    BLEServer *pServer = NULL;
    BLEStatus *status = NULL;
    BLECharacteristic *pTxCharacteristic;
    BLECharacteristic *pRxCharacteristic;
    bool deviceConnected = false;
    RxCallbacks *rxCallbacks = NULL;

public:
    BLEController();
    void sendData(String data);
    std::string getData();
};
