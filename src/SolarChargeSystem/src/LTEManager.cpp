
#include "LTEManager.h"
LTEManager::LTEManager(SerialManager& serialManager) 
{
    this->serialManager = &serialManager;
}

void LTEManager::begin() 
{
    Serial.println(F("[LTEM] Begin"));
    
    serialLTEReceiver = serialManager->getSerialLTEReceiver();
    serialLTEReceiver->setOnReceiveCallback(serialOnReceive,this);
    serialLTE = serialManager->getSerialLTE();
    serialLTE->println(F("[LTEM] Begin"));

    
    LTE.checkOK();
}

void LTEManager::run() 
{
    switch (mainProgress)
    {
    case MainProgress::ModuleSetup:
        moduleSetupLoop();
        break;
    default:
        break;
    }
    
}

void LTEManager::moduleSetupLoop()
{
    switch (setupProgress)
    {
    case SetupProgress::Main:
        LTE.begin(serialLTE);
        break;
    case SetupProgress::GetModule:
        
        break;
    case SetupProgress::GetImei:
        
        break;
    default:

        break;
    }
}


void LTEManager::serialOnReceive(void *arg, String &payload) 
{
    LTEManager& _this = *(LTEManager *)arg;
    
    _this.serialLTE->println(payload);
    
    Serial.print("[SWS] : ");
    Serial.println(payload);
    _this.LTE.onReceiveInvoke(payload);
}

