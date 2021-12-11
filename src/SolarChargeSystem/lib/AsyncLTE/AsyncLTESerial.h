/*
 * @ FileName		:
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-10 00:05:12
 * @ Modified time	: 2021-12-11 02:15:14
 * @ Description	:
 */

#ifndef __ASYNCLTESERIAL_H__
#define __ASYNCLTESERIAL_H__

#include <Arduino.h>
#include <SerialReceiver.h>
#include "SoftTimer.h"

enum class AsyncLTEReponseType;

class AsyncLTESerial : public SerialReceiver
{
    public:
    typedef void (*OnReceiveCallback)(void *arg, String &payload,AsyncLTEReponseType status);
    AsyncLTESerial() : SerialReceiver() {};
    void setOnReceiveCallback(OnReceiveCallback callback, void *arg) ;
    void clearBuffer() override;
    

    private:

    void serialCheck() override;
    
    void onReceiveCallbackInvoke(AsyncLTEReponseType status) ;
    int lineCount = 0;
    int lastLineIndex = 0;

    SoftTimer timeoutTimer;
    int timeout = 0;
    
    OnReceiveCallback onReceiveCallback;
};

#endif // __ASYNCLTESERIAL_H__