/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-25 01:15:29
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-02-28 16:11:15
 * @Description: 
 */
#include "DataLogger.h"

const char *DataLogger_head = "[SD]";
#define DATALOGGER_PRINTHEAD() Serial.print(DataLogger_head)
#define DATALOGGER_PRINT(x) Serial.print(x)
#define DATALOGGER_PRINTLN(x) Serial.println(x)
#define DATALOGGER_LOG(x)   \
    DATALOGGER_PRINTHEAD(); \
    DATALOGGER_PRINT(x)
#define DATALOGGER_LOGLN(x) \
    DATALOGGER_PRINTHEAD(); \
    DATALOGGER_PRINTLN(x)

void DataLogger::begin()
{
    if (!sdCardHelper.isReady())
        return;
    //DATALOGGER_LOG("");
    logToFileTimer.setOnExpiredCallback(logToFileCallback, this);
    logToFileTimer.setInterval(1000);

}

void DataLogger::run()
{
    logToFileTimer.run();
    
}

void DataLogger::logToFileCallback(SoftTimer &timer, void *arg)
{
    String temp;
    temp.reserve(100);
    
}
