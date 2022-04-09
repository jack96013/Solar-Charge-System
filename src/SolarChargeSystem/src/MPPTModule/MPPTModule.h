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

    float valTemp[4] = {0};
    float valTemp2[4] = {0};

private:
    uint8_t module1_address = 66;

    struct ADS1119Configuration configuration;
    ADS1119* deviceList[MPPT_DEVICES] = {0};
    ADS1119 module1 = ADS1119(byte(66));
    uint16_t module1_adcVal[4] = {0};
    float module1_zco[2] = {MPPT_I_ZCO,MPPT_I_ZCO};
    ADS1119 module2 = ADS1119(byte(64));
    uint16_t module2_adcVal[4] = {0};
    float module2_zco[2] = {MPPT_I_ZCO,MPPT_I_ZCO};
    

    SoftTimer getAdcTimer;
    static void getAdcCallback(SoftTimer &timer,void *arg);
    
    

};

#endif