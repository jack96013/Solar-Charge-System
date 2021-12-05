/*
  SerialReceiver
  @Auther ZUE-CHIEH HSU
  @Date 2021.12.04
*/

#ifndef __SERIALRECEIVERT_H__
#define __SERIALRECEIVERT_H__
#include <Arduino.h>
#include <Stream.h>

#define SM_ENDMARK '\n' 
#define SM_LENGTH 100

#define SM_DEBUG

template <class t>
class SerialReceiverT
{
    public:
    SerialReceiverT();
    void begin(t& serial,uint16_t bufferLength);
    void run();
    static void removeLineEnding(String& str);

    private:
    t& serial;

    unsigned int receiveBufferLength;
    String receiveBuffer;

    void serialCheck();
    void onNewString();
};

#endif