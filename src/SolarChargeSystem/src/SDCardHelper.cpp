#include <SDCardHelper.h>

void SDCardHelper::begin()
{
    uint32_t startMillis = millis();
    if (!sd.begin(SD_CS_PIN))
    {
        SDCARD_LOG(F("SD Card Initial Failed, ERROR : "));
        SDCARD_PRINTLN(sd.cardErrorCode());
    }
    else
    {
        ready = true;
        infoPrint();
    }
    uint32_t elapsedMillis = millis() - startMillis;
    SDCARD_LOG(F("Elapsed : "));
    SDCARD_PRINT(elapsedMillis);
    SDCARD_PRINTLN("ms");
    
}

void SDCardHelper::run()
{
}

bool SDCardHelper::isReady()
{
    return ready;
}

SdFat& SDCardHelper::getSD()
{
    return sd;
}

void SDCardHelper::errorPrint()
{
}

void SDCardHelper::infoPrint()
{
    SDCARD_LOGLN(F("SD Card Detected"));
    uint32_t cardSize = sd.card()->cardSize();
    SDCARD_LOG(F("Type : "));
    switch (sd.card()->type())
    {
    case SD_CARD_TYPE_SD1:
        SDCARD_PRINTLN(F("SD1"));
        break;

    case SD_CARD_TYPE_SD2:

        SDCARD_PRINTLN(F("SD2"));
        break;

    case SD_CARD_TYPE_SDHC:
        if (cardSize < 70000000)
            SDCARD_PRINTLN(F("SDHC"));
        else
            SDCARD_PRINTLN(F("SDXC"));
        break;
    default:
        SDCARD_PRINTLN(F("UNKNOWN"));
    }

    SDCARD_LOG(F("Format : FAT"));
    SDCARD_PRINTLN(sd.vol()->fatType());
}