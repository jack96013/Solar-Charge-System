/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2021-12-05 13:31:46
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-13 23:30:38
 * @Description: 
 */
#ifndef __LTEMANAGER_H__
#define __LTEMANAGER_H__

#include <Arduino.h>
#include <AsyncLTE.h>
#include "SerialManager.h"
#include "SerialReceiver.h"

class LTEManager
{
public:
    enum class SetupProgress
    {
        Initial,
        Check,
        CheckWait,
        MainWait,
        GetModule,
        GetImei,
        CheckRSSI,
        CheckRSSIWait,
        CheckSIM,
        CheckSIMWait,
        SetNetworkType,
        SetNetworkTypeWait,
        RegisterNetwork,
        RegisterNetworkWait,
        CheckAppNetwork,
        CheckAppNetworkWait,
        EnableAppNetwork,
        EnableAppNetworkWait,
        SetMqttParameter,
        SetMqttParameterWait,
        MqttConnect,
        MqttConnectWait,
        UNKNOWN,

    };

    LTEManager(SerialManager &manager);
    void begin();
    void run();
    void moduleSetupLoop();
    void disableDelay();
    bool isMQTTConnected();
    AsyncLTE *getLTEInstance();
    bool isNetworkActive();

    const char *getIPAddress();
    SetupProgress getProgress();
    bool isAvailabe();

private:
    enum class MainProgress
    {
        ModuleSetup,
        Server,
    };

    MainProgress mainProgress = MainProgress::ModuleSetup;
    SetupProgress setupProgress = SetupProgress::Initial;

    AsyncLTE LTE;
    SerialManager *serialManager;

    Stream *serialLTE;

    //SerialReceiver* serialLTEReceiver;
    static void serialOnReceive(void *arg, String &payload);

    SoftTimer delayTimer;

    bool mqtt_connected = false;
    bool network_active = false;

    char ipAddress[15] = "0.0.0.0";
};

#endif // __LTEMANAGER_H__