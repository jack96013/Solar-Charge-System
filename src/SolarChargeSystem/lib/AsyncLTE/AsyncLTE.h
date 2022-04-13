/*
 * @ FileName		: AsyncLTE.h
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-05 13:35:56
 * @ Modified time	: 2021-12-30 16:48:13
 * @ Description	: The SIM7000 base lib.
 */

#ifndef __ASYNCLTE_H__
#define __ASYNCLTE_H__
#include <Arduino.h>
#include <Stream.h>
#include "AsyncLTESerial.h"
#include "AsyncLTEResult.h"
#include "SoftTimer.h"

#define ASYNCLTE_OK F("OK")
#define ASYNCLTE_ERROR F("ERROR")

#define ASYNCLTE_DEBUG

#ifdef ASYNCLTE_DEBUG
#define DEBUG_PRINTHEAD() Serial.print(F("+[LTEC] "))
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTSTR(x) \
    DEBUG_PRINTHEAD();    \
    Serial.println(x)
#elif
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x) ;
#endif

enum class AsyncLTEReponseType
{
    OK,
    ERROR
};
enum class AsyncLTEState
{
    SUCCESSFUL,
    FAIL,
    BUSY
};
enum class MQTTState
{
    READY,
    SENDING
};
//
// Options
//
#define ASYNCLTE_LTEMODE_CATM1 1
#define ASYNCLTE_LTEMODE_NBIOT 2
#define ASYNCLTE_LTEMODE_BOTH 3

class AsyncLTE
{
public:
    AsyncLTE();
    void begin(Stream * serial, int8_t pwr_pin = -1, int8_t rst_pin= -1);
    void run();

    AsyncLTEState check(); // Check the module is alive;

    void powerOn();
    void powerDown();

    void setModuleBaudrate(uint16_t baudrate);
    void getModuleType();
    void getBatteryPercent();
    void getBatteryVoltage();

    AsyncLTEState requestRSSI(void);

    void getSIMStatus();

    int getRSSI(void);

    // Functionality and operation mode settings
    bool setFunctionality(uint8_t option);                                                           // AT+CFUN command
    bool enableSleepMode(bool onoff);                                                                // AT+CSCLK command
    bool set_eDRX(uint8_t mode, uint8_t connType, char *eDRX_val);                                   // AT+CEDRXS command
    bool enablePSM(bool onoff);                                                                      // AT+CPSMS command
    bool enablePSM(bool onoff, char *TAU_val, char *activeTime_val);                                 // AT+CPSMS command
    bool setNetLED(bool onoff, uint8_t mode = 0, uint16_t timer_on = 64, uint16_t timer_off = 3000); // AT+CNETLIGHT and AT+SLEDS commands

    // SIM query
    AsyncLTEState requestSIMCCID();
    int getSIMCCID(char *ccid, size_t length);

    uint8_t unlockSIM(char *pin);

    // Network
    //AsyncLTEState setPreferredMode(uint8_t mode);
    AsyncLTEState setPreferredLTEMode(uint8_t mode);
    AsyncLTEState setNetworkSettings(const __FlashStringHelper *apn, const __FlashStringHelper *username = 0, const __FlashStringHelper *password = 0);

    AsyncLTEState queryAppNetworkEnable();
    AsyncLTEState queryAppNetworkIP();
    int8_t getAppNetworkIP(char *IP, size_t length);

    AsyncLTEState enableAppNetwork(char *APN = NULL);

    uint8_t getNetworkStatus();
    AsyncLTEState enableAppNetwork();

    // IMEI
    uint8_t getIMEI(char *imei);

    // MQTT
    AsyncLTEState MQTT_connect();
    bool MQTT_requestConnectionStatus();
    AsyncLTEState MQTT_setParameter(char *URL, int16_t port = 1883, char *username = "", char *password = "", int16_t keeptime = 60);
    AsyncLTEState MQTT_publish(const char *topic, const char *message, uint16_t contentLength, byte QoS, byte retain);

    bool MQTT_subscribe();

    void onReceiveInvoke(String &data); // Push serial data to module
    void setReceiveTimeout(uint16_t timeout);

    bool isBusy();
    void checkOK();
    bool isComplete();
    bool isSuccessful();
    AsyncLTEResultBase *getResult();

    AsyncLTEState sendGeneralCommand(const char *command, uint32_t timeout = 1000);

private:
    // uint8_t pwr_pin;
    // uint8_t rst_pin;
    uint8_t timeout;
    uint8_t handler_process = 0;
    bool busy = false;

    bool waitResponse = false;

    Stream *serial;

    AsyncLTESerial serialReceiver;
    void setResultHandler(AsyncLTEResultBase *handle);
    bool createNewResultHandler();

    // AsyncLTEResultBase resultHandler;
    static void serialOnReceive(void *arg, String &payload, AsyncLTESerialState state);

    static void generalOperateOnReceive();

    static void _checkOK(void *base, String *sender, void *arg);
    // AT Commands
    void sendAT();
    void sendAT(const char *command);
    void sendAT(String *command);

    void resultHandlerStart();

    // ReceiveHandler
    static void getRssiHandler(void *base, String *sender, void *arg);

    SoftTimer resultTimeoutGuard;
    static void resultOnTimeout(SoftTimer &, void *arg);

    // Begin

    typedef struct Result_t
    {
        uint16_t timeout = 0;
        AsyncLTEState state;
    } Result;

    Result result;

    char sendBuffer[127];

    // typedef struct mqttConfigStruct {
    //     char ip[17];
    //     int16_t port;
    //     int16_t keepTime;
    //     char

    // } MQTTConfig
    MQTTState mqttState = MQTTState::READY;
};

#endif // __ASYNCLTE_H__