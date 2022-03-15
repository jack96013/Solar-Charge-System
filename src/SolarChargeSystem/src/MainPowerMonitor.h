/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-26 23:37:58
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-15 01:15:04
 * @Description: 
 */


// HSTS016L 

#ifndef __MAINPOWERMONITOR_H__
#define __MAINPOWERMONITOR_H__

#include <Arduino.h>
#include <SoftTimer.h>

#include "config/Config.h"


#define HST016L_REF_PIN         15
#define HST016L_VOUT_PIN        16
#define HST016L_GAIN_MV_PER_A   1.0f // 625mV / 30A = 

#define HST016L_CALIBRATION_TIMES_DIVIDE 10 // Calibrate Vref per 10 times of sampling.

class MainPowerMonitor
{
public:
    void begin();
    void run();

    void calibration();

    float getCurrentA();
    float getVoltage();
    float getPowerWatt();
    
private:
    int16_t refAdcVal = 0;
    int16_t currentAdcVal = 0;
    uint16_t voltageAdcVal = 0;
    uint8_t calibrationDivideTimes = 0;

    
    static void sampleCallback(SoftTimer& , void* );
    SoftTimer sampleTimer = SoftTimer(100,sampleCallback,this,SOFTTIMER_INFINITY);
    

    
    
};

#endif // __MAINPOWERMONITOR_H__