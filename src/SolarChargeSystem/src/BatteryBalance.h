#ifndef __BATTERYBALANCE_H__
#define __BATTERYBALANCE_H__

#include <Arduino.h>
#include <LTC68042.h>

class BatteryBalance
{
    public:
        BatteryBalance();
        void begin();
        void run();
        

    private:
    
};

#endif