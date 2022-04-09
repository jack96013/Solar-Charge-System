/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-25 01:15:29
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-15 17:43:20
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
    #ifdef MODULE_SD_EN
    if (!sdCardHelper.isReady())
        return;
    //DATALOGGER_LOG("");
    
    file = &sdCardHelper.getFile();

    logToFileTimer.setOnExpiredCallback(logToFileCallback, this);
    logToFileTimer.setInterval(1000);
    logToFileTimer.start();
    

    #endif
}

void DataLogger::run()
{
    logToFileTimer.run();
    
}

void DataLogger::printMainPowerData()
{
    // Index
    DATALOGGER_LOGLN(F("Record"));
    
    
    // file->print(dataIndex);
    // file->print(',');
    // file->print(mainPowerMonitor.getCurrentA());
    // file->print(',');
    // file->print(mainPowerMonitor.getVoltage());
    // file->println();
    // dataIndex ++;
    
    
    file->print(dataIndex);
    file->print(',');
    file->print(mpptModule.valTemp[0],4);
    file->print(',');
    file->print(mpptModule.valTemp[1],4);
    file->print(',');
    file->print(mpptModule.valTemp[2],4);
    file->print(',');
    file->print(mpptModule.valTemp[3],4);
    file->print(',');
    file->print(lightSensor.getValue(0),4);
    // file->print(',');
    // file->print(lightSensor.getValue(1),4);
    // file->print(',');
    // file->print(lightSensor.getValue(2),4);
    file->println();


    dataIndex ++;

    //=Serial.println(mainPowerMonitor.getCurrentA());
    
}

void DataLogger::logToFileCallback(SoftTimer &timer, void *arg)
{
    String temp;
    temp.reserve(100);
    DataLogger* _this = (DataLogger*) arg;
    
    _this->printMainPowerData();
    _this->saveDivideCount ++;
    if (_this->saveDivideCount == DATALOGGER_SAVE_DIVIDE)
    {
        _this->saveDivideCount = 0;
        DATALOGGER_LOGLN(F("Save to file"));
        _this->file->sync();
    }
}

uint32_t DataLogger::getDataCounts()
{
    return dataIndex;

}