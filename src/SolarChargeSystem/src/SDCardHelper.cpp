#include <SDCardHelper.h>

void SDCardHelper::begin()
{
    uint32_t startMillis = millis();
    
    if (!sd.begin(SD_CS_PIN))
    {
        SDCARD_LOG(F("SD Card Initial Failed, ERROR : "));
        //SDCARD_PRINTLN(sd.cardErrorCode());
        SDCARD_PRINTLN(sd.sdErrorCode());
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
    fileInitial();
    
}

void SDCardHelper::run()
{
}

bool SDCardHelper::isReady()
{
    return ready;
}

SdFat &SDCardHelper::getSD()
{
    return sd;
}

SdFile& SDCardHelper::getFile()
{
    return file;
}

void SDCardHelper::errorPrint()
{
}

void SDCardHelper::infoPrint()
{
    
    SDCARD_LOGLN(F("SD Card Detected"));
    uint32_t cardSize = sd.card()->sectorCount();
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

void SDCardHelper::fileInitial()
{
    char fileName[13] = SD_FILE_BASENAME "00." SD_FILE_TYPE;
    size_t base_name_size = sizeof(SD_FILE_BASENAME) / sizeof(char);
    while (sd.exists(fileName))
    {
        Serial.println(fileName);
        if (fileName[base_name_size] != '9')
        {
            fileName[base_name_size]++;
        }
        else if (fileName[base_name_size-1] != '9')
        {
            fileName[base_name_size] = '0';
            fileName[base_name_size-1]++;
        }
        else
        {
            SDCARD_LOGLN(F("Can't create file name!"));
            return;
        }
        
    }
    if (!file.open(fileName, O_RDWR | O_CREAT | O_TRUNC))
    {
        SDCARD_LOG(F("Can't open file - "));
        SDCARD_PRINTLN(fileName);
        return;
    }
    SDCARD_LOG(F("Open File : "));
    SDCARD_PRINTLN(fileName);
    //file.println(millis());
    //file.close();
}