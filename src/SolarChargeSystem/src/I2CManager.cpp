/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-04-09 13:28:23
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-13 21:31:20
 * @Description: 
 */
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
/**
 * @brief Scan Devices on IIC Bus
 * 
 */
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
/**
 * @brief Clear Device List
 * 
 */
void I2CManager::clearList()
{
    deviceCount = 0;
}

/**
 * @brief Check the device is online (on the list) 
 * 
 * @param address 
 * @return true 
 * @return false 
 */

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

/**
 * @brief Return the number of devices
 * 
 * @return uint8_t 
 */
uint8_t I2CManager::getDeviceCount()
{
    return deviceCount;
}