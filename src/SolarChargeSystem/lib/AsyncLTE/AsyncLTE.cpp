#include "AsyncLTE.h"

AsyncLTE::AsyncLTE()
{
}


void AsyncLTE::begin(Stream* serial, int8_t pwr_pin, int8_t rst_pin)
{
    this->serial = serial;

    //serial->print(F("AT"));
    resultTimeoutGuard.setOnExpiredCallback(resultOnTimeout, this);

    serialReceiver.begin(*serial, 100);
    serialReceiver.setOnReceiveCallback(serialOnReceive, this);

    if(pwr_pin != -1)
    {
        pwr_pin = 9;
        DEBUG_PRINTLN("PENDING");
        pinMode(pwr_pin,INPUT);
        delay(100);
        pinMode(pwr_pin,OUTPUT);
        
        digitalWrite(pwr_pin,LOW);
        
        //resultHandler.clear();
    }
    DEBUG_PRINTLN("Begin");

    
    
}
/**
 * @brief Main Loop for LTE
 * 
 */
void AsyncLTE::run()
{
    serialReceiver.run();
    resultTimeoutGuard.run();
}

/**
 * @brief Use AT Command to Check SIM7000.
 * 
 * @return AsyncLTEState 
 */
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

/**
 * @brief [NON BLOCKING] Request RSSI (Signal Strength)
 * 
 * @return AsyncLTEState 
 */
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

/**
 * @brief [NON-BLOCKING] Get RSSI after using requestRSSI Function.
 * 
 * @return int 
 */
int AsyncLTE::getRSSI()
{
    if (!serialReceiver.isOK())
        return -1;

    String *result = serialReceiver.getBuffer();

    int startIndex = result->indexOf(": ") + 1;
    int endIndex = result->indexOf(",");
    if (startIndex == -1 || endIndex == -1)
        return -1;

    int16_t val = result->substring(startIndex + 1, endIndex).toInt();
    if (val < 0 || val > 31)
        return -1;

    val = map(val, 0, 31, 0, 100);
    return val;
}

bool AsyncLTE::setFunctionality(uint8_t option)
{
}

void AsyncLTE::getRssiHandler(void *base, String *sender, void *arg)
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

/**
 * @brief [NON-BLOCKING] Request SIM Card CCID
 * 
 * @return AsyncLTEState 
 */
AsyncLTEState AsyncLTE::requestSIMCCID()
{
    return sendGeneralCommand("CCID");
}

int AsyncLTE::getSIMCCID(char *ccid, size_t length)
{
    if (!serialReceiver.isOK())
        return -1;

    String *result = serialReceiver.getBuffer();
    strncpy(ccid, result->c_str(), length);

    return 1;
}

AsyncLTEState AsyncLTE::setPreferredLTEMode(uint8_t mode)
{
    char command[8] = "";
    strcat(command, "CMNB=");
    char charValue = mode + '0';
    strncat(command, &charValue, 1);
    Serial.println(command);
    return sendGeneralCommand(command, 2000);
}

AsyncLTEState AsyncLTE::setNetworkSettings(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password)
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

AsyncLTEState AsyncLTE::queryAppNetworkIP()
{
    //     [21:49:45.154]楷↙⊙AT+CNACT?
    // ↓
    // [21:49:45.158]彶↘↑AT+CNACT?
    // +CNACT: 1,"10.161.254.177"

    // OK

    return sendGeneralCommand("CNACT?", 2000);
}

int8_t AsyncLTE::getAppNetworkIP(char *IP, size_t length)
{
    if (!serialReceiver.isOK())
        return -1;

    const char *result = serialReceiver.getBuffer()->c_str();
    int8_t result_len = strlen(result);
    int active = result[8] - 48;
    if (active != 1)
        return -1;
    int startIndex = 11;
    int len = result_len - startIndex - 1;
    memcpy(IP, &result[startIndex], len);
    IP[len] = '\0';
    return 1;
}

AsyncLTEState AsyncLTE::enableAppNetwork(char *APN)
{
    char command[30];
    sprintf(command, "CNACT=1,\"%s\"", APN);
    Serial.println(command);

    return sendGeneralCommand(command, 2000);
}

AsyncLTEState AsyncLTE::MQTT_connect()
{
    return sendGeneralCommand("SMCONN", 5000);
}

AsyncLTEState AsyncLTE::MQTT_setParameter(char *URL, int16_t port, char *username, char *password, int16_t keeptime)
{
    if (isBusy())
        return AsyncLTEState::BUSY;
    // strcat(command,"\"URL\"=\"");
    // strcat(command,URL);
    // strcat(command,"\",");
    char command[50];
    sprintf(command, "AT+SMCONF=\"URL\",\"%s\",%d;", URL, port);
    serial->print(command);

    sprintf(command, "+SMCONF=\"USERNAME\",\"%s\";", username);
    serial->print(command);

    sprintf(command, "+SMCONF=\"PASSWORD\",\"%s\";", password);
    serial->print(command);

    sprintf(command, "+SMCONF=\"KEEPTIME\",\"%d\"", keeptime);
    serial->println(command);

    serialReceiver.startWaitForResult(100);

    return AsyncLTEState::SUCCESSFUL;
}

AsyncLTEState AsyncLTE::MQTT_publish(const char *topic, const char *message, uint16_t contentLength, byte QoS, byte retain)
{
    if (isBusy())
        return AsyncLTEState::BUSY;

    sprintf(sendBuffer, "SMPUB=\"%s\",%i,%i,%i", topic, contentLength, QoS, retain);
    // Send Command
    sendGeneralCommand(sendBuffer);
    // Wait for ">" (Non-Blocking)
    strcpy(sendBuffer, message);
    mqttState = MQTTState::SENDING;

    return AsyncLTEState::SUCCESSFUL;

    // MQTT
    // 1. SEND AT COMMAND
    // 2. WAIT FOR ">"  (in Receive Callback)
    // 3. SEND MESSAGE  (..)
    // 4. WAIT FOR "OK" (..)
    // 5. FINISH
}

void AsyncLTE::onReceiveInvoke(String &data)
{
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

void AsyncLTE::setResultHandler(AsyncLTEResultBase *handle)
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

void AsyncLTE::serialOnReceive(void *arg, String &payload, AsyncLTESerialState state)
{
    AsyncLTE *_this = (AsyncLTE *)arg;

    DEBUG_PRINTHEAD();

    DEBUG_PRINT(F("OK , RESPONSE TYPE:"));
    if (state == AsyncLTESerialState::OK)
    {
        //_this->resultHandler.returnSuccessful();
        DEBUG_PRINT(F("OK , RECEIVE :"));
        DEBUG_PRINTLN(payload);

        if (_this->mqttState == MQTTState::SENDING)
        {
            _this->mqttState = MQTTState::READY;
            DEBUG_PRINTLN(F("WAIT FOR MQTT OK"));
            _this->serialReceiver.startWaitForResult(2000);
            _this->serial->println(_this->sendBuffer);
        }
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

void AsyncLTE::sendAT(const char *command)
{
    serial->print(F("AT"));

    DEBUG_PRINTHEAD();
    DEBUG_PRINT(F("-> AT"));

    if (strcmp(command, ""))
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

void AsyncLTE::sendAT(String *command)
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

AsyncLTEResultBase *AsyncLTE::getResult()
{
    return nullptr;
}

AsyncLTEState AsyncLTE::sendGeneralCommand(const char *command, uint32_t timeout)
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
void AsyncLTE::resultOnTimeout(SoftTimer &, void *arg)
{
    AsyncLTE *_this = (AsyncLTE *)arg;
    _this->result.state = AsyncLTEState::FAIL;
}

//
// SIM Card
//
