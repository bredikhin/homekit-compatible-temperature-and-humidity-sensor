#include <Arduino.h>
#include <CurieBLE.h>
#include "DHT.h"

#define DHTPIN 2 // data pin
#define DHTTYPE DHT22

// Initialize the sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize BLE
BLEPeripheral ble;
BLEService informationService("180A");
BLECharacteristic modelCharacteristic("2A24", BLERead, "101");
BLECharacteristic manufacturerCharacteristic("2A29", BLERead, "Arduino");
BLECharacteristic serialNumberCharacteristic("2A25", BLERead, "2.71828");

// Services and characteristics
BLEService humidityService("10525F60-CF73-11E6-9598-F8E2251C9A69");
BLEFloatCharacteristic humidityCharacteristic("10525F61-CF73-11E6-9598-F8E2251C9A69", BLERead | BLENotify);
BLEService thermometerService("F489CB00-C177-11E6-9598-9854249C9A66");
BLEFloatCharacteristic temperatureCharacteristic("F489CB01-C177-11E6-9598-9854249C9A66", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  dht.begin();

  ble.setLocalName("ArduinoSensor");
  ble.setAdvertisedServiceUuid(thermometerService.uuid());
  ble.addAttribute(informationService);
  ble.addAttribute(modelCharacteristic);
  ble.addAttribute(manufacturerCharacteristic);
  ble.addAttribute(serialNumberCharacteristic);

  ble.addAttribute(humidityService);
  ble.addAttribute(humidityCharacteristic);
  humidityCharacteristic.setValueLE(0.0);
  ble.addAttribute(thermometerService);
  ble.addAttribute(temperatureCharacteristic);
  temperatureCharacteristic.setValueLE(0.0);

  ble.setEventHandler(BLEConnected, centralConnect);
  ble.setEventHandler(BLEDisconnected, centralDisconnect);

  ble.begin();
  Serial.println("Bluetooth on");
}

void loop() {
  // We can't read data from the sensor more often
  // than every 2 seconds
  delay(2000);

  ble.poll();

  // Humidity
  float humidity = dht.readHumidity();
  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity)) {
    Serial.println("Failed to read humidity from DHT sensor!");
    return;
  }
  float prevHumidity = humidityCharacteristic.valueLE();
  if (abs(humidity - prevHumidity) > 1.50) {
    humidityCharacteristic.setValueLE(humidity);
    Serial.print("Update humidity | ");
    Serial.print(humidity, 0);
    Serial.println("%");
  } else {
    Serial.print("Humidity | ");
    Serial.print(humidity, 0);
    Serial.println("%");
  }

  // Temperature
  float celsiusTemp = dht.readTemperature();
  // Check if any reads failed and exit early (to try again)
  if (isnan(celsiusTemp)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    return;
  }
  float prevCelsiusTemp = temperatureCharacteristic.valueLE();
  if (abs(celsiusTemp - prevCelsiusTemp) > 0.20) {
    temperatureCharacteristic.setValueLE(celsiusTemp);
    Serial.print("Update temperature | ");
    Serial.print(celsiusTemp, 2);
    Serial.println("*C");
  } else {
    Serial.print("Temperature | ");
    Serial.print(celsiusTemp, 2);
    Serial.println("*C");
  }
}

void centralConnect(BLECentral& central) {
  Serial.print("Central connected | ");
  Serial.println(central.address());
}

void centralDisconnect(BLECentral& central) {
  Serial.print("Central disconnected | ");
  Serial.println(central.address());
}
