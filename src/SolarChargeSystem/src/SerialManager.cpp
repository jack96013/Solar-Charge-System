

#include "SerialManager.h"
#include "config\Config.h"

SerialManager::SerialManager()
{
}

void SerialManager::begin()
{
    Serial.begin(SERIAL_BAUD);
    SerialLTE.begin(LTE_SERIAL_BAUD);

    printInfo(Serial);
    //printInfo(SerialLTE);

    serialReceiver.begin(Serial, SERIAL_BUFFER_LEN);
    //serialLTEReceiver.begin(SerialLTE, LTE_SERIAL_BUFFER_LEN);

    serialReceiver.setOnReceiveCallback(serialOnReceive, this);
    //serialLTEReceiver.setOnReceiveCallback(serialLTEOnReceive, this);
}

void SerialManager::run()
{
    serialReceiver.run();
    //serialLTEReceiver.run();
}

void SerialManager::printInfo(Stream &serial)
{
    serial.print("\n");
    serial.print(F("==========================\n"));
    serial.print(F(" Solar Charge System\n"));
    serial.print("\n");
    serial.print(F(" Powered by ECIE 110 Team\n"));
    serial.print(F(" Build : "));
    serial.println(F(__TIMESTAMP__));
    serial.print(F("=========================\n"));
}

AltSoftSerial* SerialManager::getSerialLTE()
{
    return &SerialLTE;
}

// SerialReceiver* SerialManager::getSerialLTEReceiver() 
// {
//     return &serialLTEReceiver;
// }

void SerialManager::serialOnReceive(void *arg, String &payload)
{
    SerialManager* _this = arg;
    Serial.println("[SEND]");
    Serial.println(payload);
    _this->SerialLTE.println(payload);
}

// void SerialManager::serialLTEOnReceive(void *arg, String &payload)
// {
//     SerialManager &_this = *(SerialManager *)arg;
//     _this.SerialLTE.println(payload);
//     if (payload.indexOf("AT2") != 0)
//         _this.failCount++;
//     _this.SerialLTE.println(_this.failCount);

//     Serial.print("[SWS] : ");
//     Serial.println(payload);
// }