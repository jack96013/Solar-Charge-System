#include <AsyncLTE.h>

AsyncLTE::AsyncLTE()
{
}

AsyncLTEResultBase* AsyncLTE::begin(Stream *serial)
{
    this->serial = serial;

    DEBUG_PRINTLN("Begin");
    sendAT();
    serial->print(F("AT"));
    

    resultHandler = new AsyncLTEResultBase(nullptr,this);
    return resultHandler;


}

void AsyncLTE::begin(Stream *, uint8_t pwr_pin, uint8_t rst_pin)
{
}

void AsyncLTE::run()
{
}

void AsyncLTE::powerOn()
{
}

void AsyncLTE::powerDown()
{
}

void AsyncLTE::setModuleBaudrate(uint16_t baudrate)
{
}

void AsyncLTE::getModuleType()
{
}

void AsyncLTE::getBatteryPercent()
{
}

void AsyncLTE::getBatteryVoltage()
{
}

bool AsyncLTE::enableSleepMode(bool onoff)
{
}

bool AsyncLTE::set_eDRX(uint8_t mode, uint8_t connType, char *eDRX_val)
{
}

bool AsyncLTE::enablePSM(bool onoff)
{
}

bool AsyncLTE::enablePSM(bool onoff, char *TAU_val, char *activeTime_val)
{
}

bool AsyncLTE::setNetLED(bool onoff, uint8_t mode, uint16_t timer_on, uint16_t timer_off)
{
}

uint8_t AsyncLTE::getSIMCCID(char *ccid)
{
}

void AsyncLTE::onReceiveInvoke(String &data)
{
    if (resultHandler)
        resultHandler->onReceivedInvoke(&data);
}

void AsyncLTE::setReceiveTimeout(uint16_t timeout)
{
}

bool AsyncLTE::isBusy()
{
    return resultHandler != nullptr;
}

AsyncLTEResult<bool> *AsyncLTE::checkOK()
{
    if (isBusy())
        return nullptr;

    serial->println("AT");

    AsyncLTEResult<bool> *newResult = new AsyncLTEResult<bool>(_checkOK, this);
    this->resultHandler = (AsyncLTEResultBase *)(newResult);
    return newResult;
}

void AsyncLTE::_checkOK(void *base, String *sender, void *arg)
{
    AsyncLTEResult<bool> *result = (AsyncLTEResult<bool>*)base;

    if (sender->equals("AT-OK"))
    {
        Serial.println("Successful");
        result->returnSuccessful();

        
    }
}

void AsyncLTE::setResultHandler(AsyncLTEResultBase* handle)
{
    if (handle != nullptr)
        return;

    if (resultHandler != nullptr)
    {
        delete resultHandler;
    }
    resultHandler = handle;
            
}

bool AsyncLTE::createNewResultHandler()
{
    if (!isBusy())
        return false;

}

void AsyncLTE::sendAT()
{
    serial->println("AT");
}