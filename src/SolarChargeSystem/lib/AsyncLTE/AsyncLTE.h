/*
 * @ FileName		: AsyncLTE.h
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-05 13:35:56
 * @ Modified time	: 2021-12-11 16:10:14
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

#define DEBUG_PRINTLN(x)  Serial.print(F("LTEC")); Serial.println(F((x)));

enum class AsyncLTEReponseType{OK,ERROR};
enum class AsyncLTEState{SUCCESSFUL,FAIL,BUSY};


class AsyncLTE
{
public:
    

    AsyncLTE();
    AsyncLTEResultBase* begin(Stream *);
    void begin(Stream *, uint8_t pwr_pin, uint8_t rst_pin);
    void run();

    AsyncLTEState check();   // Check the module is alive;

    void powerOn();
    void powerDown();

    void setModuleBaudrate(uint16_t baudrate); 
    void getModuleType();
    void getBatteryPercent();
    void getBatteryVoltage();
    
    AsyncLTEState getRSSI(void);
    int getRSSIValue(void);

    // Functionality and operation mode settings
    bool setFunctionality(uint8_t option);                                                           // AT+CFUN command
    bool enableSleepMode(bool onoff);                                                                // AT+CSCLK command
    bool set_eDRX(uint8_t mode, uint8_t connType, char *eDRX_val);                                   // AT+CEDRXS command
    bool enablePSM(bool onoff);                                                                      // AT+CPSMS command
    bool enablePSM(bool onoff, char *TAU_val, char *activeTime_val);                                 // AT+CPSMS command
    bool setNetLED(bool onoff, uint8_t mode = 0, uint16_t timer_on = 64, uint16_t timer_off = 3000); // AT+CNETLIGHT and AT+SLEDS commands

    // SIM query
    
    uint8_t unlockSIM(char *pin);
    uint8_t getSIMCCID(char *ccid);
    uint8_t getNetworkStatus();
    

    // IMEI
    uint8_t getIMEI(char *imei);

    void onReceiveInvoke(String& data); // Push serial data to module
    void setReceiveTimeout(uint16_t timeout);

    bool isBusy();

    void checkOK();
    
    bool isComplete();
    bool isSuccessful();
    AsyncLTEResultBase* getResult();
    
private:
    uint8_t pwr_pin;
    uint8_t rst_pin;
    uint8_t timeout;
    uint8_t handler_process = 0;
    bool busy = false;
    
    bool waitResponse = false;

    Stream *serial;
    
    AsyncLTESerial serialReceiver;
    void setResultHandler(AsyncLTEResultBase* handle);
    bool createNewResultHandler();
    
    AsyncLTEResultBase resultHandler;
    static void serialOnReceive(void *arg, String &payload,AsyncLTEReponseType status);
    
    static void generalOperateOnReceive();

    static void _checkOK(void* base,String* sender,void* arg);
    // AT Commands
    void sendAT();
    void sendAT(const char* command);
    void sendAT(String* command);
    
    void resultHandlerStart();
    
    // ReceiveHandler
    static void getRssiHandler(void* base,String* sender,void* arg);
    
    SoftTimer resultTimeoutGuard;
    static void resultOnTimeout(SoftTimer&,void* arg);

    // Begin
    
    
};





#endif // __ASYNCLTE_H__