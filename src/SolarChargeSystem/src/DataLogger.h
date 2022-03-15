/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-25 01:15:16
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-15 17:31:24
 * @Description: 
 */
#ifndef __DATALOGGER_H__
#define __DATALOGGER_H__

#include <Arduino.h>
#include <SoftTimer.h>
#include "config/Config.h"
#include "MainPowerMonitor.h"

extern MainPowerMonitor mainPowerMonitor;

#ifdef MODULE_SD_EN
#include "SDCardHelper.h"
extern SDCardHelper sdCardHelper;
#endif

#define DATALOGGER_SAVE_DIVIDE 3

class DataLogger
{
public:
    void begin();
    void run();
    void printMainPowerData();

private:
    SoftTimer logToFileTimer;
    static void logToFileCallback(SoftTimer &timer, void *arg);
    SdFile* file;
    uint32_t dataIndex = 0;
    uint16_t saveDivideCount = 0;
};

#endif