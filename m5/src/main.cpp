#include <Arduino.h>
#include <M5Unified.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TX "beb5483f-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a9"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
BLECharacteristic *pRxCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
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
};

#define TEMP_SENSOR_PIN 35
#define GEAR_SENSOR_PIN 36

// put function declarations here:
int getTemperature();
int getGearPos();
void BLESetup();

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
  BLESetup();
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.print("Temp: ");
  Serial.println(getTemperature());
  Serial.print("Gear: ");
  int gear = getGearPos();
  if (deviceConnected)
  {
    Serial.println("send");
    pTxCharacteristic->setValue(std::to_string(gear));
    pTxCharacteristic->notify();
  }
  M5.Lcd.setCursor(10, 70);
  M5.Display.print(gear);
  delay(100);
}

double fixVoltage(double voltage)
{
  return voltage * 5000 / 1100;
}

// put function definitions here:
int getTemperature()
{
  double voltage = analogReadMilliVolts(TEMP_SENSOR_PIN) / 1000.0;
  double fixedVoltage = fixVoltage(voltage);
  M5.Lcd.setCursor(10, 10);
  M5.Display.print(fixedVoltage);
  double volt_data[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; // {6乗, 5乗, ４乗, 3乗, 2乗、1乗}
  for (int i = 0; i < 6; i++)
  {
    for (int n = 0; n < (6 - i); n++)
    {
      volt_data[i] *= fixedVoltage; // 最終的に {w_volt_avr^6, w_volt_avr^5, w_volt_avr^4, w_volt_avr^3, w_volt_avr^2, w_volt_avr}
    }
  }
  double w_data = (0.1067 * volt_data[0]) - (2.6257 * volt_data[1]) + (22.627 * volt_data[2]) - (93.172 * volt_data[3]) + (199.71 * volt_data[4]) - (241.82 * volt_data[5]) + 202.85; // 202.82などの数値についてはMoteCの温度センサーに付いてくる資料を見よ
  return w_data;
}

int getGearPos()
{
  double voltage = analogReadMilliVolts(GEAR_SENSOR_PIN) / 1000.0;
  double fixedVoltage = fixVoltage(voltage);
  M5.Lcd.setCursor(10, 40);
  M5.Display.print(fixedVoltage);
  Serial.println("volt" + String(fixedVoltage));

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
  case 380 ... 400:
    return 6;
  case 405 ... 500:
    return 0;
  default:
    return 0;
  }
}

void BLESetup()
{
  BLEDevice::init("ESP32_BLE_SERVER"); // この名前がスマホなどに表示される
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
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