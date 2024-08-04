#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "ble.hpp"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TX "beb5483f-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a9"

std::string readData = "Hello World!";
std::string filePath = "";
std::string nowTime = "";
std::string latitude = "";
std::string longitude = "";
std::string speed = "";
std::string heading = "";

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

class RxCallbacks : public BLECharacteristicCallbacks
{

private:
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0)
        {
            size_t start = 0;
            size_t end = rxValue.find(',');

            // 各変数に順番に値を格納
            if (end != std::string::npos)
            {
                filePath = rxValue.substr(start, end - start);
                start = end + 1;
                end = rxValue.find(',', start);
            }

            if (end != std::string::npos)
            {
                nowTime = rxValue.substr(start, end - start);
                start = end + 1;
                end = rxValue.find(',', start);
            }

            if (end != std::string::npos)
            {
                latitude = rxValue.substr(start, end - start);
                start = end + 1;
                end = rxValue.find(',', start);
            }

            if (end != std::string::npos)
            {
                longitude = rxValue.substr(start, end - start);
                start = end + 1;
                end = rxValue.find(',', start);
            }

            if (end != std::string::npos)
            {
                speed = rxValue.substr(start, end - start);
                start = end + 1;
                end = rxValue.find(',', start);
            }

            if (end != std::string::npos)
            {
                heading = rxValue.substr(start, end - start);
            }
            else
            {
                heading = rxValue.substr(start);
            }
        }
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
    pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE); // 読み書き可能にする

    rxCallbacks = new RxCallbacks();
    pRxCharacteristic->setCallbacks(rxCallbacks);
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

std::string BLEController::getData()
{
    return readData;
}

std::string getFilePath()
{
    return filePath;
}

std::string getTime()
{
    return nowTime;
}

std::string getLatitude()
{
    return latitude;
}

std::string getLongitude()
{
    return longitude;
}

std::string getSpeed()
{
    return speed;
}

std::string getHeading()
{
    return heading;
}