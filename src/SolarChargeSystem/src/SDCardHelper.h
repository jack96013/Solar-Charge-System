#ifndef __SDCARDHELPER_H__
#define __SDCARDHELPER_H__

#include <Arduino.h>
#include <SdFat.h>
#include <SoftTimer.h>

#include "config/Config.h"
#include "log.h"

const char *SD_LOG_HEAD = "SD";

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

    private:
        enum class SetupProgress
        {
            Initial,
            Info,
        };

        SdFat sd;
        bool ready = false;

        void errorPrint();       
        void infoPrint(); 
        //SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(16));
};

#endif