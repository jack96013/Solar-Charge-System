

// HSTS016L 

#ifndef __MAINPOWERMONITOR_H__
#define __MAINPOWERMONITOR_H__

#include <Arduino.h>
#include <SoftTimer.h>

#include "config/Config.h"


#define MAINPOWER_ADC_REF       5.00f
#define MAINPOWER_VIN_PIN       A2
#define MAINPOWER_VIN_GAIN      1.0f

#define HST016L_REF_PIN         A0
#define HST016L_VOUT_PIN        A1
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
    uint16_t refAdcVal = 0;
    uint16_t currentAdcVal = 0;
    uint16_t voltageAdcVal = 0;
    uint8_t calibrationDivideTimes = 0;

    
    static void sampleCallback(SoftTimer& , void* );
    SoftTimer sampleTimer = SoftTimer(100,sampleCallback,this,SOFTTIMER_INFINITY);
    

    
    
};

#endif // __MAINPOWERMONITOR_H__