/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-25 01:15:16
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-26 20:30:06
 * @Description: 
 */
#ifndef __DATALOGGER_H__
#define __DATALOGGER_H__

#include <Arduino.h>
#include <SoftTimer.h>
#include "config/Config.h"
#include "MainPowerMonitor.h"
#include "MPPTModule/MPPTModule.h"
extern MainPowerMonitor mainPowerMonitor;
extern MPPTModule mpptModule;
#include "SDCardHelper.h"

#ifdef MODULE_SD_EN
extern SDCardHelper sdCardHelper;
#endif

#define DATALOGGER_SAVE_DIVIDE 10

class DataLogger
{
public:
    void begin();
    void run();
    void printMainPowerData();
    uint32_t getDataCounts();

private:
    SoftTimer logToFileTimer;
    static void logToFileCallback(SoftTimer &timer, void *arg);
    SdFile* file;
    uint32_t dataIndex = 0;
    uint16_t saveDivideCount = 0;
    
};

#endif