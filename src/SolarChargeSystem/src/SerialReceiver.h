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

using namespace std;

class SerialReceiver
{
    public:
    typedef void (*OnReceiveCallback)(void* arg, String& payload);

    SerialReceiver();
    void begin(Stream& serial,uint16_t bufferLength);
    void run();
    static void removeLineEnding(String& str);
    void setOnReceiveCallback(OnReceiveCallback callback,void* arg);
    

    private:
    Stream* serial;
    

    unsigned int receiveBufferLength;
    String receiveBuffer;

    void serialCheck();
    void onReceiveCallbackInvoke();
    OnReceiveCallback onReceiveCallback;
    void* onReceiveCallbackArg;
};

#endif