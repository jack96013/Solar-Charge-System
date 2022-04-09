#include "I2CManager.h"

void I2CManager::begin()
{

    Serial.println("Start Scanning");
    lastMillis = millis();
    scanDevices();
    Serial.print(F("Device Count "));
    Serial.println(deviceCount);
    Serial.print(F("Elapsed"));
    Serial.println(millis() - lastMillis);
}

void I2CManager::run()
{

}
void I2CManager::scanDevices()
{
    for (uint8_t addr = 1; addr < I2CMANAGER_MAX_ADDR; addr++)
    {
        Wire.beginTransmission(addr);
        uint8_t result =  Wire.endTransmission();
        if (result != 0)
            continue;
        deviceList[deviceCount] = addr;
        deviceCount++;
    }
}
void I2CManager::clearList()
{
    deviceCount = 0;
}

bool I2CManager::isAvailable(uint8_t address)
{
    for (uint8_t i = 0; i < deviceCount; i++)
    {
        uint8_t addr = deviceList[i];
        if (addr = address)
            return true;
    }
    return false;
}