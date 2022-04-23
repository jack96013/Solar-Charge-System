/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-15 19:14:06
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-24 03:30:48
 * @Description: 
 */
#ifndef _MPPTMODULE_H__
#define _MPPTMODULE_H__
#include "ADS1119.h"
#include "config/Config.h"
#include "SoftTimer.h"
#include "I2CManager.h"

#define MPPT_PRINTHEAD()   Serial.print(F("[MPPT]"))
#define MPPT_PRINT(x)      Serial.print(x)
#define MPPT_PRINTLN(x)    Serial.println(x)
#define MPPT_LOG(x)   MPPT_PRINTHEAD(); MPPT_PRINT(x)
#define MPPT_LOGLN(x)   MPPT_PRINTHEAD(); MPPT_PRINTLN(x)


extern I2CManager i2cManager;

class MPPTModule
{
public:
    void begin();
    void run();
    void scanDevices();
    void calibrate();

    

    float getInputVoltage(uint8_t index);
    float getInputCurrent(uint8_t index);
    float getOutputVoltage(uint8_t index);
    float getOutputCurrent(uint8_t index);
    

private:
    uint8_t address[MPPT_DEVICES] = {64,65,66,67,68,69};
    ADS1119 adc;
    struct ADS1119Configuration configuration;
    

    float zeroCurrentVoltage[MPPT_DEVICES][2] = {0};
    float valTemp[MPPT_DEVICES][4] = {0};

    SoftTimer getAdcTimer;
    static void getAdcCallback(SoftTimer &timer,void *arg);
    
    float readVoltage(uint8_t index,uint8_t channel);
    void saveToTemp(uint8_t index,float Vin,float Iin,float Vout,float Iout);
    void readMpptModule(uint8_t index);

    int readIndex = 3;
    enum Channel
    {
        MPPT_VIN,
        MPPT_IIN,
        MPPT_IOUT,
        MPPT_VOUT,
        LENGTH,
    };

    uint8_t currentChannel = MPPT_VIN;
    bool onCalibrate = false;

    struct ADS1119Configuration calibration;
};

#endif