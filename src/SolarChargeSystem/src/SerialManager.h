/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2021-12-04 20:03:28
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-23 23:32:17
 * @Description: 
 */
/*
 * @ FileName		: SerialManager.cpp
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-04 20:03:18
 * @ Modified time	: 2021-12-10 01:17:48
 * @ Description	: Manager 2 serial interface
 */

#ifndef __SERIALMANAGER_H__
#define __SERIALMANAGER_H__

#include <Arduino.h>
#ifdef __AVR__
#include <AltSoftSerial.h>
#endif

#include "SerialReceiverTemp.h"
#include <SerialReceiver.h>
#include "BatteryBalance/BatteryBalance.h"
#include "config/Config.h"

class SerialManager
{
public:
    SerialManager();

    void begin();
    void run();
    void printInfo(Stream &serial);
    Stream* getSerialLTE();
    SerialReceiver* getSerialReceiver();
    
private:

    Stream* SerialLTE; // LTE Module
    SerialReceiver serialReceiver;
    //SerialReceiver serialLTEReceiver;
    
    #ifdef __AVR_ATmega328P__
    AltSoftSerial softSerial;
    #endif

    // Callback
    static void serialOnReceive(void *arg, String &payload);
    //static void serialLTEOnReceive(void *arg, String &payload);

    int failCount = 0;

    //SerialReceiverT<HardwareSerial> serialReceiver;
    //SerialReceiverT<SoftwareSerial> serialLTEReceiver;
};

#endif