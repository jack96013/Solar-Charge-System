#ifndef __LTEMANAGER_H__
#define __LTEMANAGER_H__

#include <Arduino.h>
#include <AsyncLTE.h>
#include "SerialManager.h"
#include "SerialReceiver.h"

class LTEManager
{
    public:
    LTEManager(SerialManager& manager);
    void begin();
    void run();
    void moduleSetupLoop();
    void disableDelay();

    private:
    enum class MainProgress {
        ModuleSetup,
        Server,
    };

    enum class SetupProgress {
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
        
        UNKNOWN,
        
    };


    MainProgress mainProgress = MainProgress::ModuleSetup;
    SetupProgress setupProgress = SetupProgress::Initial;

    AsyncLTE LTE;
    SerialManager* serialManager;
    AltSoftSerial* serialLTE;
    // Stream* serialLTE;

    //SerialReceiver* serialLTEReceiver;
    static void serialOnReceive(void *arg, String &payload);

    SoftTimer delayTimer;

};

#endif // __LTEMANAGER_H__