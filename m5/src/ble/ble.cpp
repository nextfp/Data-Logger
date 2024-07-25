#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "ble.hpp"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TX "beb5483f-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a9"

class BLEStatus : public BLEServerCallbacks
{
private:
    bool deviceConnected = false;

public:
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        // Serial.println("Connected");
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
        // Serial.println("Disconnected");
    }

    bool isConnected()
    {
        return deviceConnected;
    }
};

BLEController::BLEController()
{
    BLEDevice::init("ESP32_BLE_SERVER"); // この名前がスマホなどに表示される
    pServer = BLEDevice::createServer();
    status = new BLEStatus();
    pServer->setCallbacks(status);
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
    pTxCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // iPhone接続の問題に役立つ
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}

void BLEController::sendData(String data)
{
    if (status->isConnected())
    {
        pTxCharacteristic->setValue(data.c_str());
        pTxCharacteristic->notify();
    }
}
