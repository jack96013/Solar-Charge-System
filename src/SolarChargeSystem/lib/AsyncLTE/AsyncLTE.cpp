#include "AsyncLTE.h"

AsyncLTE::AsyncLTE()
{
}

AsyncLTEResultBase* AsyncLTE::begin(Stream *serial)
{
    this->serial = serial;

    
    //serial->print(F("AT"));
    resultTimeoutGuard.setOnExpiredCallback(resultOnTimeout,this);
    
    serialReceiver.begin(*serial,100);
    serialReceiver.setOnReceiveCallback(serialOnReceive,this);


    DEBUG_PRINTLN("Begin");
    
    resultHandler.clear();

    return &resultHandler;


}

void AsyncLTE::begin(Stream *, uint8_t pwr_pin, uint8_t rst_pin)
{
}

void AsyncLTE::run()
{
    serialReceiver.run();
    resultTimeoutGuard.run();
}

AsyncLTEState AsyncLTE::check() 
{
    if (isBusy())
        return AsyncLTEState::BUSY;
    sendAT();
    resultHandler.clear();
    resultHandler.start();
    serialReceiver.clearBuffer();
    return AsyncLTEState::SUCCESSFUL;
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

AsyncLTEState AsyncLTE::getRSSI()
{
    if (isBusy())
        return AsyncLTEState::BUSY;
    sendAT("CSQ");

    serialReceiver.clearBuffer();
    resultHandler.clear();
    resultHandler.setOnReceived(getRssiHandler,this);
    resultHandler.start();
    
    return AsyncLTEState::SUCCESSFUL;
}

void AsyncLTE::getRssiHandler(void* base,String* sender,void* arg)
{
    AsyncLTEResultBase *result = (AsyncLTEResultBase*)base;
    
    int startIndex = sender->indexOf(": ")+1;
    int endIndex = sender->indexOf(",");
    if (startIndex == -1 || endIndex == -1)
    {
        result->returnFail();
        return;
    }
        
    int16_t val = sender->substring(startIndex+1,endIndex).toInt();
    if (val < 0 || val > 31)
    {
        result->returnResult(0);
        return;
    }
    Serial.print("[current]");
    val = map(val,0,31,0,100);
    Serial.println(val);
    result->returnResult(val);
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
    // if (resultHandler)
    //     resultHandler->onReceivedInvoke(&data);
    resultHandler.onReceivedInvoke(&data);
}

void AsyncLTE::setReceiveTimeout(uint16_t timeout)
{
}

bool AsyncLTE::isBusy()
{
    return !resultHandler.isCompleted();
}

// AsyncLTEResult<bool> *AsyncLTE::checkOK()
// {
//     if (isBusy())
//         return nullptr;

//     serial->println("AT");

//     AsyncLTEResult<bool> *newResult = new AsyncLTEResult<bool>(_checkOK, this);
//     this->resultHandler = (AsyncLTEResultBase *)(newResult);
//     return newResult;
// }

// void AsyncLTE::_checkOK(void *base, String *sender, void *arg)
// {
//     AsyncLTEResult<bool> *result = (AsyncLTEResult<bool>*)base;

//     if (sender->equals("AT-OK"))
//     {
//         Serial.println("Successful");
//         result->returnSuccessful();

        
//     }
// }

void AsyncLTE::setResultHandler(AsyncLTEResultBase* handle)
{
    // if (handle != nullptr)
    //     return;

    // if (resultHandler != nullptr)
    // {
    //     //delete resultHandler;
    // }
    //resultHandler = handle;
            
}

bool AsyncLTE::createNewResultHandler()
{
    if (!isBusy())
        return false;

}


void AsyncLTE::serialOnReceive(void *arg, String &payload,AsyncLTEReponseType status)
{
    AsyncLTE* _this = (AsyncLTE*)arg;
    if (status == AsyncLTEReponseType::OK)
    {
        _this->resultHandler.returnSuccessful();
        Serial.print("[OK]");
    }    
    else
    {
        _this->resultHandler.returnFail();
        Serial.print("[ERROR]");
    }
    Serial.println(payload);

    _this->resultHandler.onReceivedInvoke(&payload);
    
    //_this->serial.println(payload);
}

void AsyncLTE::sendAT()
{
    sendAT("");
    
}

void AsyncLTE::sendAT(const char* command)
{
    serial->print(F("AT"));
    Serial.println(F("AT"));
    if (strcmp(command,""))
    {
        Serial.print(F("+"));
        serial->print(F("+"));
        serial->println(command);
    }
    else
    serial->println();
}

void AsyncLTE::sendAT(String* command)
{
    sendAT(command->c_str());
}

bool AsyncLTE::isComplete()
{
    return resultHandler.isCompleted();
}

bool AsyncLTE::isSuccessful()
{
    return !resultHandler.isFail();
}

AsyncLTEResultBase* AsyncLTE::getResult()
{
    return &resultHandler;
}

void resultOnTimeout(SoftTimer&,void* arg)
{
    AsyncLTE* _base = (AsyncLTE*)arg;
    _base->resultHandler.clear();
}

//
// SIM Card
//

