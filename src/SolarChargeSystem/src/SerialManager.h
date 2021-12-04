#ifndef __SERIALMANAGER_H__
#define __SERIALMANAGER_H__

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "SerialReceiver.h"
#include "config/Config.h"

class SerialManager
{
    public:
    SerialManager();

    void printInfo();

    private:
    SoftwareSerial Serial1 = SoftwareSerial(2,3);
    SerialReceiver serialReceiver = SerialReceiver((Stream&)Serial);
    SerialReceiver serialReceiver1 = SerialReceiver((Stream&)Serial1);
};

#endif