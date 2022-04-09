#ifndef __I2CMANAGER_H__
#define __I2CMANAGER_H__
#include <Arduino.h>
#include <Wire.h>
#include <SoftTimer.h>

#define I2CMANAGER_MAX_ADDR 127
#define I2CMANAGER_MIN_ADDR 1
#define I2CMANAGER_MAX_DEVICES 10

#define I2CMANAGER_SCAN_INTERVAL 0      // 0 SCAN FIRSTTIME ONLY
#define I2CMANAGER_DEVICE_INTERVAL 0    // 0 without delay

class I2CManager{
    public:
        void begin();
        void run();
        void scanDevices();
        void clearList();

        bool isAvailable(uint8_t address);
    private:
        uint8_t deviceList[I2CMANAGER_MAX_DEVICES];
        uint8_t deviceCount = 0;
        uint8_t currentAddress = 0;
        uint8_t scanDelayPerAddress = 0;


        void appendDevice(uint8_t address);

        SoftTimer scanTimer;
    
        int lastMillis = 0;
};
#endif