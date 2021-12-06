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
    

    private:
    enum class MainProgress {
        ModuleSetup,
        Server,
    };

    enum class SetupProgress {
        Main,
        MainWait,
        GetModule,
        GetImei,
    };


    MainProgress mainProgress = MainProgress::ModuleSetup;
    SetupProgress setupProgress = SetupProgress::Main;

    AsyncLTE LTE;
    SerialManager* serialManager;
    Stream* serialLTE;
    SerialReceiver* serialLTEReceiver;
    static void serialOnReceive(void *arg, String &payload);

    

};

#endif // __LTEMANAGER_H__