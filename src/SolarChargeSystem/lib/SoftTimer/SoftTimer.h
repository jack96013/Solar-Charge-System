/*
 * @ FileName		: SoftTimer.cpp
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-05 14:13:11
 * @ Modified time	: 2021-12-10 23:28:57
 * @ Description	: A software timer implemented with millis();
 */

#ifndef __SOFTTIMER_H__
#define __SOFTTIMER_H__
#include <Arduino.h>

#define SOFTTIMER_INFINITY 0

class SoftTimer
{
    public:
    typedef void (*SoftTimerCallback)(SoftTimer&,void* arg);

    SoftTimer(uint32_t interval = 0, SoftTimerCallback onExpiredCallback = NULL ,void* callbackArg=nullptr , uint32_t repeat = SOFTTIMER_INFINITY);
    //SoftTimer(uint32_t interval, SoftTimerCallback onExpiredCallback ,void* callbackArg);
    //SoftTimer(uint32_t interval, SoftTimerCallback onExpiredCallback ,uint32_t repeat);
    //SoftTimer(uint32_t interval, SoftTimerCallback onExpiredCallback);
    void setOnExpiredCallback(SoftTimerCallback onExpiredCallback,void* arg);
    void setInterval(uint32_t interval);
    void setRepeats(uint32_t repeat);
    uint32_t getTargetTime();

    void run();
    void delay(uint32_t delay);

    void stop();
    void start();
    void startFrom(uint32_t startTime);
    void reset();
    void forceExpired();

    bool isRunning();
    bool isExpired();
    
    uint32_t getRemainingTime();
    uint32_t getRemainingRepeats();  // Number of repeats remaining.

    void clearSettings();

  private:
    enum { RUNNING, STOPPED, EXPIRED } state;
    
    uint32_t startTime;
    uint32_t remainingTime;
    uint32_t interval;

    uint32_t repeatTarget;
    uint32_t repeatCount;
    SoftTimerCallback onExpiredCallback;
    void* onExpiredCallbackArg = nullptr;

    void onExpiredInvoke();

};


#endif
