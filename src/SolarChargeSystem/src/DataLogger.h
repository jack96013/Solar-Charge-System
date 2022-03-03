#ifndef __DATALOGGER_H__
#define __DATALOGGER_H__

#include <Arduino.h>
#include <SoftTimer.h>
#include "SDCardHelper.h"

extern SDCardHelper sdCardHelper;

class DataLogger
{
public:
    void begin();
    void run();

private:
    SoftTimer logToFileTimer;
    static void logToFileCallback(SoftTimer &timer, void *arg);
};

#endif