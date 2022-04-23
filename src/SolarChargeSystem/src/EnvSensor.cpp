/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-04-09 06:28:59
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-16 09:49:59
 * @Description:
 */
#include "EnvSensor.h"

void EnvSensor::begin()
{
    getDataTimer.setOnExpiredCallback(getDataCallback, this);
    getDataTimer.setInterval(1000);
    getDataTimer.start();
#if defined(ENV_BH1750_EN)
    lightMeter[0].begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);
#endif
#if defined(ENV_MAX44009_EN)
    for (int i = 0; i < ENV_LIGHT_MAX_DEVICE; i++)
    {
        lightMeter[i].setContinuousMode();
        lightMeter[i].setAutomaticMode();
    }
#endif
    bmp.begin();

    // if (!bmp.begin())
    ///    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
}

void EnvSensor::run()
{
    getDataTimer.run();
}

void EnvSensor::getDataCallback(SoftTimer &timer, void *arg)
{
    EnvSensor *_this = (EnvSensor *)arg;
    for (int i = 0; i < 1; i++)
    {
#ifdef ENV_BH1750_EN
        _this->luxTemp[i] = _this->lightMeter[i].readLightLevel();
        _this->autoAdjust(i);
#endif

#ifdef ENV_MAX44009_EN

        float lux = _this->lightMeter[i].getLux();
        int err = _this->lightMeter[i].getError();
        if (err == 0)
            _this->luxTemp[i] = lux;
        
#endif
    }
    Serial.println(_this->luxTemp[0]);
    _this->temperature = _this->bmp.readTemperature();
}

void EnvSensor::autoAdjust(int sensorIndex)
{
#if defined(ENV_BH1750_EN)
    if (luxTemp[sensorIndex] > 40000.0)
        lightMeter[sensorIndex].setMTreg(32);
    else if (luxTemp[sensorIndex] > 10.0)
        lightMeter[sensorIndex].setMTreg(69);
    else if (luxTemp[sensorIndex] <= 10.0)
        lightMeter[sensorIndex].setMTreg(138);
#endif
}

float EnvSensor::getValue(int index)
{
    return luxTemp[index];
}

float EnvSensor::getTemperature()
{
    return temperature;
}

float EnvSensor::toWM2(uint16_t lux)
{
    return lux * 0.0079;
}