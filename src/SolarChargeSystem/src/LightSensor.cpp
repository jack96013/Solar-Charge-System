/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-04-09 06:28:59
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-12 06:42:34
 * @Description: 
 */
#include "LightSensor.h"

void LightSensor::begin()
{
    getDataTimer.setOnExpiredCallback(getLuxCallback, this);
    getDataTimer.setInterval(1000);
    getDataTimer.start();

    lightMeter[0].begin(BH1750::CONTINUOUS_HIGH_RES_MODE,0x23);
    Serial.println("OK");
    delay(10);
    //lightMeter[1].begin(BH1750::CONTINUOUS_HIGH_RES_MODE,0x5c);
    Serial.println("OK");

    //if (!bmp.begin()) 
    ///    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
}

void LightSensor::run()
{
    getDataTimer.run();
}

void LightSensor::getLuxCallback(SoftTimer &timer, void *arg)
{
    LightSensor *_this = (LightSensor *)arg;
    _this->luxTemp[0] = _this->lightMeter[0].readLightLevel();
    //_this->luxTemp[1] = _this->lightMeter[1].readLightLevel();
    //bool readFinish = _this->lightMeter[0].measurementReady()
    
    // Serial.print(_this->luxTemp[0]);
    // Serial.print(" ");
    // Serial.println(_this->luxTemp[1]);
    //_this->autoAdjust(0);
    //_this->autoAdjust(1);
    // float temperature = _this->bmp.readTemperature();
    // float pressure = _this->bmp.readPressure();
    // Serial.print("Temperature = ");
    // Serial.print(temperature);
    // Serial.println(" *C");
    
    // Serial.print("Pressure = ");
    // Serial.print(pressure);
    // Serial.println(" Pa");
}

void LightSensor::autoAdjust(int sensorIndex)
{
        if (luxTemp[sensorIndex] > 40000.0)
            lightMeter[sensorIndex].setMTreg(32);
        else if (luxTemp[sensorIndex] > 10.0)
            lightMeter[sensorIndex].setMTreg(69);
        else if (luxTemp[sensorIndex] <= 10.0)
            lightMeter[sensorIndex].setMTreg(138);
}

float LightSensor::getValue(int index)
{
    return luxTemp[index];
}

float LightSensor::toWM2(uint16_t lux)
{
    return lux * 126.58;
}