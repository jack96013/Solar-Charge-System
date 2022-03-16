/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-23 23:37:51
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-15 02:32:17
 * @Description: 
 */
#ifndef __SDCARDHELPER_H__
#define __SDCARDHELPER_H__

#include <Arduino.h>
#include <SdFat.h>
#include <SoftTimer.h>

#include "config/Config.h"
#include "log.h"


#define SDCARD_PRINTHEAD()     Serial.print(F("[SD] "))
#define SDCARD_PRINT(x)      Serial.print(x)
#define SDCARD_PRINTLN(x)    Serial.println(x)
#define SDCARD_LOG(x)   SDCARD_PRINTHEAD(); SDCARD_PRINT(x)
#define SDCARD_LOGLN(x)   SDCARD_PRINTHEAD(); SDCARD_PRINTLN(x)



class SDCardHelper
{
    public:
        void begin();
        void run();
        void printInfo();
        void log();
        bool isReady();
    

        SdFat& getSD();
        SdFile& getFile();
        char* getFileName();

        
    private:
        enum class SetupProgress
        {
            Initial,
            Info,
        };

        SdFat sd;
        SdFile file;
        char fileName[15] = SD_FILE_BASENAME "00." SD_FILE_TYPE;

        bool ready = false;

        void errorPrint();       
        void infoPrint(); 
        void fileInitial();

        //SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(16));
};

#endif