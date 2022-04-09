#ifndef __LIGHTSENSOR_H__
#define __LIGHTSENSOR_H__
#include <Arduino.h>
#include <SoftTimer.h>
#include "config/Config.h"
#include "BH1750.h"
#include "Adafruit_BMP085.h"

class LightSensor
{
    public:
        void run();
        void begin();
        
        bool isReady();

        static float toWM2(uint16_t lux);
        void autoAdjust(int sensorIndex);
        float getValue(int index);
    private:
        BH1750 lightMeter[2];

        bool ready = false;
        SoftTimer getDataTimer = SoftTimer();
        static void getLuxCallback(SoftTimer &timer , void* arg);
        // Data
        static const int8_t luxDevices = 3;
        float luxTemp[luxDevices]={0};

        

};

#endif



  