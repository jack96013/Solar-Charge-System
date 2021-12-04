/*
  SerialReceiver
  @Auther ZUE-CHIEH HSU
  @Date 2021.12.04
  

*/
#ifndef __SERIALRECEIVER_H__
#define __SERIALRECEIVER_H__
#include <Arduino.h>
#include <Stream.h>

#define SM_ENDMARK '\n' 
#define SM_LENGTH 100

#define SM_DEBUG

class SerialReceiver
{
    public:
    SerialReceiver(Stream& serial);
    void begin(uint16_t bufferLength);
    void run();
    static void removeLineEnding(String& str);

    private:
    Stream& serial;

    unsigned int receiveBufferLength;
    String receiveBuffer;

    void serialCheck();
    void onNewString();
};

#endif