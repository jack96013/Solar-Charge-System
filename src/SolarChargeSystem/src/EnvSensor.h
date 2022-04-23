/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-04-09 06:29:18
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-16 09:45:33
 * @Description:
 */
#ifndef __ENVSENSOR_H__
#define __ENVSENSOR_H__
#include <Arduino.h>
#include <SoftTimer.h>
#include "config/Config.h"
#include "BH1750.h"
#include "Adafruit_BMP085.h"
#include <Max44009.h>

//#define ENV_BH1750_EN
#define ENV_MAX44009_EN
#define ENV_LIGHT_MAX_DEVICE 2

class EnvSensor
{
public:
    void run();
    void begin();

    bool isReady();

    static float toWM2(uint16_t lux);
    void autoAdjust(int sensorIndex);
    float getValue(int index);
    float getTemperature();

private:
    // ***** Light Sensor *****
#if defined(ENV_BH1750_EN)
    BH1750 lightMeter[2];
#endif
#if defined(ENV_MAX44009_EN)
    Max44009 lightMeter[ENV_LIGHT_MAX_DEVICE] = {
        Max44009(MAX44009_DEFAULT_ADDRESS),
        Max44009(MAX44009_ALT_ADDRESS)};
#endif

    bool ready = false;
    SoftTimer getDataTimer = SoftTimer();
    static void getDataCallback(SoftTimer &timer, void *arg);
    // Data
    float luxTemp[ENV_LIGHT_MAX_DEVICE] = {0};


    // ***** Temperature Sensor *****
    Adafruit_BMP085 bmp;

    float temperature = 25;
};

#endif
