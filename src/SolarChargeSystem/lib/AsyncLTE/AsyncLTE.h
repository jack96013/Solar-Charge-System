#ifndef __ASYNCLTE_H__
#define __ASYNCLTE_H__
#include <Arduino.h>
#include <Stream.h>

class AsyncLTE
{
public:
    AsyncLTE();
    void begin(Stream *);
    void begin(Stream *, uint8_t pwr_pin, uint8_t rst_pin);

    void run();

    void powerOn();
    void powerDown();

    void setModuleBaudrate(uint16_t baudrate);
    void getModuleType();
    void getBatteryPercent();
    void getBatteryVoltage();

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
    uint8_t getNetworkStatus(void);
    uint8_t getRSSI(void);

    // IMEI
    uint8_t getIMEI(char *imei);

    void onReceiveInvoke(String& data); // Push serial data to module
    void setReceiveTimeout(uint16_t timeout);
private:
    uint8_t pwr_pin;
    uint8_t rst_pin;
    uint8_t timeout;
    
    bool waitResponse = false;

    Stream *serial;
};

#endif