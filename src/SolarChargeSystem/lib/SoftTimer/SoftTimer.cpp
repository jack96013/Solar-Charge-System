#include "SoftTimer.h"

SoftTimer::SoftTimer(uint32_t interval, SoftTimerCallback onExpiredCallback, void *callbackArg, uint32_t repeat)
{
    this->interval = interval;
    this->onExpiredCallback = onExpiredCallback;
    this->onExpiredCallbackArg = callbackArg;
    this->repeatTarget = repeat;
}

void SoftTimer::setOnExpiredCallback(SoftTimerCallback onExpiredCallback, void *arg)
{
    this->onExpiredCallback = onExpiredCallback;
    this->onExpiredCallbackArg = arg;
}

void SoftTimer::setInterval(uint32_t interval)
{
    this->interval = interval;
}

void SoftTimer::setRepeats(uint32_t repeat)
{
    this->repeatTarget = repeat;
}

void SoftTimer::run()
{
    if (isRunning())
    {

        if (state == EXPIRED)
        {
            if (repeatTarget == SOFTTIMER_INFINITY)
            {
                state = RUNNING;
                
            }
            else if (repeatCount == 0)
                state = STOPPED;
                
        }
        if (millis() - startTime < interval)
            return;

        startTime = millis();
        state = EXPIRED;

        if (repeatTarget != SOFTTIMER_INFINITY)
            repeatCount--;
        onExpiredInvoke();
    }
}

void SoftTimer::delay(uint32_t delay)
{
    setInterval(delay);
    setRepeats(1);
    start();
}

void SoftTimer::stop()
{
    state = STOPPED;
}

void SoftTimer::start()
{
    startFrom(millis());
}

void SoftTimer::startFrom(uint32_t startTime)
{
    this->state = RUNNING;
    this->startTime = startTime;
    this->repeatCount = repeatTarget;
}

void SoftTimer::reset()
{
    startFrom(millis());
}

void SoftTimer::forceExpired()
{
    startTime = millis() + interval;
}

bool SoftTimer::isRunning()
{
    // if (repeatTarget == SOFTTIMER_INFINITY)
    // {
    //     return state == RUNNING || state == EXPIRED;
    // }
    return state != STOPPED;
}

bool SoftTimer::isExpired()
{
    return state == EXPIRED;
}

void SoftTimer::onExpiredInvoke()
{
    if (onExpiredCallback)
        onExpiredCallback(*this, onExpiredCallbackArg);
}

void SoftTimer::clearSettings()
{
    this->interval = 0;
    this->onExpiredCallback = nullptr;
    this->onExpiredCallbackArg = nullptr;
    this->repeatTarget = INFINITY;
}

uint32_t SoftTimer::getRemainingTime()
{
    return (startTime + interval) - millis();
}