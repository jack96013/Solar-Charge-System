#ifndef _MPPTMODULE_H__
#define _MPPTMODULE_H__
#include "ADS1119.h"
#include "config/Config.h"
#include "SoftTimer.h"

#define MPPT_PRINTHEAD()   Serial.print(F("[MPPT]"))
#define MPPT_PRINT(x)      Serial.print(x)
#define MPPT_PRINTLN(x)    Serial.println(x)
#define MPPT_LOG(x)   MPPT_PRINTHEAD(); MPPT_PRINT(x)
#define MPPT_LOGLN(x)   MPPT_PRINTHEAD(); MPPT_PRINTLN(x)

class MPPTModule
{
public:
    void begin();
    void run();
    void scanDevices();

private:
    struct ADS1119Configuration configuration;
    ADS1119* deviceList[MPPT_DEVICES] = {0};
    ADS1119 module1 = ADS1119(byte(64));
    uint16_t module1_adcVal[4] = {0};


    SoftTimer getAdcTimer;
    static void getAdcCallback(SoftTimer &timer,void *arg);
    
    

};

#endif