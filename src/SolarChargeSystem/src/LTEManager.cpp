
#include "LTEManager.h"
LTEManager::LTEManager(SerialManager& serialManager) 
{
    this->serialManager = &serialManager;
}

void LTEManager::begin() 
{
    Serial.println(F("[LTEM] Begin"));
    
    //serialLTEReceiver = serialManager->getSerialLTEReceiver();
    //serialLTEReceiver->setOnReceiveCallback(serialOnReceive,this);
    serialLTE = serialManager->getSerialLTE();
    //serialLTE->println(F("[LTEM] Begin"));

    serialLTE->begin(57600);
    LTE.begin(serialLTE);
    
    //LTE.checkOK();
}

void LTEManager::run() 
{
    LTE.run();
    delayTimer.run();
    
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
    // set networksetting
    switch (setupProgress)
    {
    case SetupProgress::Initial:
    {
        delayTimer.clearSettings();
        delayTimer.delay(15000);     // 等待開機
        //Serial.println(delayTimer.getRemainingTime());
        setupProgress=SetupProgress::Check;
        break;
    }
    case SetupProgress::Check:      // 檢查模塊
    {
        if (!delayTimer.isExpired())
            return;
        Serial.println(F("[LTE] Checking..."));
        
        AsyncLTEState state = LTE.check();
        if (state == AsyncLTEState::SUCCESSFUL)
            setupProgress=SetupProgress::CheckWait;
        else
            setupProgress=SetupProgress::Initial;
        break;
    }
    case SetupProgress::CheckWait:
    {
        if (!LTE.isComplete())
            return;
        if (LTE.isSuccessful())
        {
            Serial.println(F("[LTE] Module is Ready"));
            setupProgress=SetupProgress::CheckRSSI;
        }
        else
        {
            Serial.println(F("[LTE] Module is Fail,retrying..."));
            setupProgress = SetupProgress::Check;
        }
        
        break;
    }
    case SetupProgress::CheckRSSI:  // 檢查訊號
        Serial.println(F("[LTE] Get Signal"));
        LTE.getRSSI();
        setupProgress = SetupProgress::CheckRSSIWait;
        break;
    case SetupProgress::CheckRSSIWait:
        if (!LTE.isComplete())
            return;
        if (LTE.isSuccessful())
        {
            Serial.print(F("[LTE] Signal Strength : "));
            setupProgress=SetupProgress::CheckRSSI;
            int val = LTE.getResult()->getInt();
            Serial.println(val);
        }
        setupProgress = SetupProgress::CheckSIM;
        break;
    case SetupProgress::CheckSIM:   // 檢查 SIM Card
        //LTE.getSIM();
        break;
    case SetupProgress::RegisterNetwork:    // 註冊網路
        break;
    
    
    default:

        break;
    }
}


void LTEManager::serialOnReceive(void *arg, String &payload) 
{
    LTEManager& _this = *(LTEManager *)arg;
    
    //_this.serialLTE->println(payload);
    
    Serial.print("[SWS] : ");
    Serial.println(payload);
    //_this.LTE.onReceiveInvoke(payload);
}

