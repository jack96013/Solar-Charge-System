#include <DataLogger.h>

const char* DataLogger_head = "[SD]";
#define DATALOGGER_PRINTHEAD()     Serial.print(DataLogger_head)
#define DATALOGGER_PRINT(x)      Serial.print(x)
#define DATALOGGER_PRINTLN(x)    Serial.println(x)
#define DATALOGGER_LOG(x)   DATALOGGER_PRINTHEAD(); DATALOGGER_PRINT(x)
#define DATALOGGER_LOGLN(x)   DATALOGGER_PRINTHEAD(); DATALOGGER_PRINTLN(x)


void DataLogger::begin()
{
    if (!sdCardHelper.isReady())
        return;
    

}

void DataLogger::run()
{
    
}
