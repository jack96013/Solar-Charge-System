#ifndef __SERIALMANAGER_H__
#define __SERIALMANAGER_H__

#include <Arduino.h>
#include <AltSoftSerial.h>

#include "SerialReceiverTemp.h"
#include "SerialReceiver.h"
#include "config/Config.h"

class SerialManager
{
public:
    SerialManager();

    void begin();
    void run();
    void printInfo(Stream &serial);
    AltSoftSerial getSerialLTE();

private:
    AltSoftSerial SerialLTE; // LTE Module
    SerialReceiver serialReceiver;
    SerialReceiver serialLTEReceiver;


    // Callback
    static void serialOnReceive(void *arg, String &payload);
    static void serialLTEOnReceive(void *arg, String &payload);

    int failCount = 0;

    //SerialReceiverT<HardwareSerial> serialReceiver;
    //SerialReceiverT<SoftwareSerial> serialLTEReceiver;
};

#endif