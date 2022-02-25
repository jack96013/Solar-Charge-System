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
    
    //resultHandler.clear();

    return nullptr;


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
    //resultHandler.clear();
    //resultHandler.start();
    serialReceiver.startWaitForResult();
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

AsyncLTEState AsyncLTE::requestRSSI()
{
    if (isBusy())
        return AsyncLTEState::BUSY;
    sendAT("CSQ");

    serialReceiver.startWaitForResult();

    return AsyncLTEState::SUCCESSFUL;
}


void AsyncLTE::getSIMStatus() 
{
    
}

int AsyncLTE::getRSSI()
{
    if (!serialReceiver.isOK())
        return -1;
    
    String* result = serialReceiver.getBuffer();
    
    int startIndex = result->indexOf(": ")+1;
    int endIndex = result->indexOf(",");
    if (startIndex == -1 || endIndex == -1)
        return -1;
        
    int16_t val = result->substring(startIndex+1,endIndex).toInt();
    if (val < 0 || val > 31)
        return -1;

    val = map(val,0,31,0,100);
    return val;
}

bool AsyncLTE::setFunctionality(uint8_t option) 
{
    
}

void AsyncLTE::getRssiHandler(void* base,String* sender,void* arg)
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

AsyncLTEState AsyncLTE::requestSIMCCID()
{
    return sendGeneralCommand("CCID");
}

int AsyncLTE::getSIMCCID(char *ccid,size_t length)
{
    if (!serialReceiver.isOK())
        return -1;
    
    String* result = serialReceiver.getBuffer();
    strncpy(ccid,result->c_str(),length);
    
    return 1;
}

AsyncLTEState AsyncLTE::setPreferredLTEMode(uint8_t mode)
{
    char command[8]="";
    strcat(command,"CMNB=");
    char charValue = mode+'0';
    strncat(command,&charValue,1);
    Serial.println(command);
    return sendGeneralCommand(command,2000);
}

AsyncLTEState AsyncLTE::setNetworkSettings(const __FlashStringHelper* apn, const __FlashStringHelper* username=0, const __FlashStringHelper* password=0)
{
    //AT+CGDCONT=1,"IP","internet.iot"
    //F("AT+CGDCONT=1,\"IP\",");
    if (isBusy())
        return AsyncLTEState::BUSY;

    serial->print(F("AT+"));
    serial->print(F("CGDCONT=1,\"IP\","));
    serial->print('\"');
    serial->print(apn);
    serial->println('\"');
    

    serialReceiver.startWaitForResult(2000);
    return AsyncLTEState::SUCCESSFUL;
}


void AsyncLTE::onReceiveInvoke(String &data)
{
    // if (resultHandler)
    //     resultHandler->onReceivedInvoke(&data);
    // resultHandler.onReceivedInvoke(&data);
}

void AsyncLTE::setReceiveTimeout(uint16_t timeout)
{
}

bool AsyncLTE::isBusy()
{
    return serialReceiver.getState() == AsyncLTESerialState::WAITING;
    //return !resultHandler.isCompleted();
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


void AsyncLTE::serialOnReceive(void *arg, String &payload,AsyncLTESerialState state)
{
    AsyncLTE* _this = (AsyncLTE*)arg;

    DEBUG_PRINTHEAD();

    DEBUG_PRINT(F("OK , RESPONSE TYPE:"));
    if (state == AsyncLTESerialState::OK)
    {
        //_this->resultHandler.returnSuccessful();
        DEBUG_PRINT(F("OK , RECEIVE :"));
        DEBUG_PRINTLN(payload);
    }    
    else
    {
        //_this->resultHandler.returnFail();
        
        DEBUG_PRINTLN(F("ERROR"));
    }
    

    //_this->resultHandler.onReceivedInvoke(&payload);
    
    //_this->serial.println(payload);
}

void AsyncLTE::sendAT()
{
    sendAT("");
    
}

void AsyncLTE::sendAT(const char* command)
{
    serial->print(F("AT"));
    
    DEBUG_PRINTHEAD();
    DEBUG_PRINT(F("-> AT"));
    
    if (strcmp(command,""))
    {
        DEBUG_PRINT(F("+"));
        DEBUG_PRINT(command);

        serial->print(F("+"));
        serial->println(command);
    }
    else
        serial->println();

    DEBUG_PRINTLN();
}

void AsyncLTE::sendAT(String* command)
{
    sendAT(command->c_str());
}

bool AsyncLTE::isComplete()
{
    return !isBusy();
}

bool AsyncLTE::isSuccessful()
{
    return serialReceiver.isOK();
}

AsyncLTEResultBase* AsyncLTE::getResult()
{
    return nullptr;
}

AsyncLTEState AsyncLTE::sendGeneralCommand(const char* command,uint32_t timeout)
{
    if (isBusy())
        return AsyncLTEState::BUSY;
    sendAT(command);
    serialReceiver.startWaitForResult(timeout);
    return AsyncLTEState::SUCCESSFUL;
}

//
// When the request is timeout
//
void AsyncLTE::resultOnTimeout(SoftTimer&,void* arg)
{
    AsyncLTE* _this = (AsyncLTE*)arg;
    _this->result.state = AsyncLTEState::FAIL;
}

//
// SIM Card
//

