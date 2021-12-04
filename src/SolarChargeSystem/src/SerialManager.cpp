#include "SerialManager.h"

SerialManager::SerialManager()
{
    
}

void SerialManager::printInfo()
{
    Serial.print("\n");
    Serial.print(F("==========================\n"));
    Serial.print(F(" Solar Charge System\n"));
    Serial.print("\n");
    Serial.print(F(" Powered by ECIE 110 Team\n"));
    Serial.print(F(" Build : "));
    Serial.println(F(__TIMESTAMP__));
    Serial.print(F("=========================\n"));
}