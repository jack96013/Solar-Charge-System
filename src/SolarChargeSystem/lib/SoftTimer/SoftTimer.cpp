
#include "SoftTimer.h"

SoftTimer::SoftTimer(uint32_t interval = 0, SoftTimerCallback onExpiredCallback = NULL ,void* callbackArg=nullptr , uint32_t repeat = SOFTTIMER_INFINITY)
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
    if (state == RUNNING)
    {
        if (millis() - startTime < interval)
            return;
        else
        {
            startTime = millis();
            if (repeatCount != SOFTTIMER_INFINITY)
            {
                repeatCount--;
                if (repeatCount == 0)
                    state = EXPIRED;
            }
            onExpiredInvoke();
        }
    }
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
    state = RUNNING;
    startTime = millis();
    repeatCount = repeatTarget;
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
    return state == RUNNING;
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
