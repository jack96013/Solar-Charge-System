/*
 * @ FileName		:
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-10 00:05:12
 * @ Modified time	: 2021-12-30 16:46:44
 * @ Description	:
 */

#ifndef __ASYNCLTESERIAL_H__
#define __ASYNCLTESERIAL_H__

#include <Arduino.h>
#include <SerialReceiver.h>
#include "SoftTimer.h"

enum class AsyncLTEReponseType;
enum class AsyncLTESerialState {WAITING,TIMEOUT,OK,ERROR};

#define ASYNCLTESERIAL_TIMEOUT 1000

class AsyncLTESerial : public SerialReceiver
{
    public:
    typedef void (*OnReceiveCallback)(void *arg, String &payload,AsyncLTESerialState status);
    AsyncLTESerial() : SerialReceiver() {};
    void setOnReceiveCallback(OnReceiveCallback callback, void *arg) ;
    void clearBuffer() override;
    void setEnable(bool en);
    bool isReceived();
    bool isOK();
    
    void startWaitForResult(uint32_t timeout = ASYNCLTESERIAL_TIMEOUT);
    AsyncLTESerialState getState();


    
    private:
    bool enable = true;
    
    void serialCheck() override;
    
    void onReceiveCallbackInvoke() ;
    int lineCount = 0;
    int lastLineIndex = 0;

    SoftTimer timeoutTimer;
    
    OnReceiveCallback onReceiveCallback;

    AsyncLTESerialState state = AsyncLTESerialState::OK;
};

#endif // __ASYNCLTESERIAL_H__