/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2021-12-04 20:03:18
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-07 21:23:36
 * @Description: 
 */


#include "SerialManager.h"
// 使用軟體 Serial
#if defined(ARDUINO_ARCH_AVR) 
#include "AltSoftSerial.h" 
#endif


SerialManager::SerialManager()
{
}

void SerialManager::begin()
{
    Serial.begin(115200);
    

    #if defined(ARDUINO_ARCH_AVR) 
        softSerial.begin(LTE_SERIAL_BAUD);
        SerialLTE = &softSerial;
    #elif defined(ARDUINO_ARCH_STM32F1)
        Serial1.begin(115200);
        Serial2.begin(LTE_SERIAL_BAUD);
        SerialLTE = &Serial2;
        
    #endif
    
    printInfo(DebugSerial);
    //printInfo(SerialLTE);

    //serialReceiver.begin(DebugSerial, SERIAL_BUFFER_LEN);
    //serialLTEReceiver.begin(SerialLTE, LTE_SERIAL_BUFFER_LEN);

    //serialReceiver.setOnReceiveCallback(serialOnReceive, this);
    //serialLTEReceiver.setOnReceiveCallback(serialLTEOnReceive, this);
}

void SerialManager::run()
{
    //serialReceiver.run();
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

Stream* SerialManager::getSerialLTE()
{
    return SerialLTE;
}

// SerialReceiver* SerialManager::getSerialLTEReceiver() 
// {
//     return &serialLTEReceiver;
// }

void SerialManager::serialOnReceive(void *arg, String &payload)
{
    SerialManager* _this = (SerialManager*)arg;
    Serial.println("[SEND]");
    Serial.println(payload);
    //_this->SerialLTE.println(payload);
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