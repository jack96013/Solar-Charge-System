
#include "LTEManager.h"
LTEManager::LTEManager(SerialManager &serialManager)
{
    this->serialManager = &serialManager;
}

void LTEManager::begin()
{
    Serial.println(F("[LTEM] Begin! Wait for module reboot"));

    //serialLTEReceiver = serialManager->getSerialLTEReceiver();
    //serialLTEReceiver->setOnReceiveCallback(serialOnReceive,this);
    serialLTE = serialManager->getSerialLTE();
    //serialLTE->println(F("[LTEM] Begin"));

    serialLTE->begin(57600);
    LTE.begin(serialLTE);

    
    delayTimer.setInterval(100);
    delayTimer.setRepeats(SOFTTIMER_INFINITY);
    delayTimer.start();

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
    
    if(!delayTimer.isExpired())
    {
        return;
    }
    
    AsyncLTEState state;
    // set networksetting
    switch (setupProgress)
    {
    case SetupProgress::Initial:
    {
        Serial.println(F("[LTE] Initial Start"));

        delayTimer.delay(5000); // 等待開機
        //Serial.println(delayTimer.getRemainingTime());
        setupProgress = SetupProgress::Check;
        break;
    }
    case SetupProgress::Check: // 檢查模塊
    {
        Serial.println(F("[LTE] Checking..."));

        state = LTE.check();
        
        setupProgress = SetupProgress::CheckWait;
        
        disableDelay(); // 關閉延遲執行
        break;
    }
    case SetupProgress::CheckWait:
    {
        if (!LTE.isComplete())
            return;
        if (LTE.isSuccessful())
        {
            Serial.println(F("[LTE] Module is Ready"));
            setupProgress = SetupProgress::CheckRSSI;

            disableDelay();
        }
        else
        {
            Serial.println(F("[LTE] Module is Fail,retrying..."));
            setupProgress = SetupProgress::Check;
            delayTimer.delay(2000); // 延遲一秒後重新 Check
            
        }

        break;
    }
    case SetupProgress::CheckRSSI: // 檢查訊號
    {
        Serial.println(F("[LTE] Get Signal"));
        LTE.requestRSSI();
        setupProgress = SetupProgress::CheckRSSIWait;
        disableDelay();
        break;
    }
    case SetupProgress::CheckRSSIWait:
    {
        if (!LTE.isComplete())
            return;
        if (LTE.isSuccessful())
        {
            Serial.print(F("[LTE] Signal Strength : "));
            setupProgress = SetupProgress::CheckSIM;
            int val = LTE.getRSSI();
            Serial.println(val);
        }
        else
        {
            setupProgress = SetupProgress::CheckRSSI;
            delayTimer.delay(2000); // 延遲一秒後重新 Check
        }
        break;
    }
    case SetupProgress::CheckSIM: // 取得 SIM Card 卡號 (檢查是否有插卡)
    {
        Serial.println(F("[LTE] Get SIM CCID"));
        state = LTE.requestSIMCCID();
        
        setupProgress = SetupProgress::CheckSIMWait;

        disableDelay();
    }
    case SetupProgress::CheckSIMWait: // 取得 SIM Card 卡號 (檢查是否有插卡)
    {
        if (!LTE.isComplete())
            return;
        if (LTE.isSuccessful())
        {
            Serial.print(F("[LTE] SIM Detected : "));
            size_t length = 21;
            char ccid[length] = "";
            LTE.getSIMCCID((char*) ccid,length);
            Serial.println(ccid);
            
            setupProgress = SetupProgress::SetNetworkType;
        }
        else
        {
            setupProgress = SetupProgress::CheckSIM;
            delayTimer.delay(2000); // 延遲一秒後重新 Check
        }
           
        break;
    }
    case SetupProgress::SetNetworkType:
    {
        Serial.println(F("[LTE] Set Network Type"));
        state = LTE.setPreferredLTEMode(ASYNCLTE_LTEMODE_CATM1);

        setupProgress = SetupProgress::SetNetworkTypeWait;
        disableDelay();
        break;
    }
    case SetupProgress::SetNetworkTypeWait:
    {
        if (!LTE.isComplete())
            return;
        if (LTE.isSuccessful())
        {
            Serial.println(F("[LTE] Set Successful"));
            setupProgress = SetupProgress::RegisterNetwork;
        }
        else
        {
            setupProgress = SetupProgress::SetNetworkType;
            delayTimer.delay(2000); // 延遲一秒後重新 Check
        }
        break;
    }
    case SetupProgress::RegisterNetwork:
    {
        Serial.println(F("[LTE] Register APN"));
        state = LTE.setNetworkSettings(F("internet.iot"));
        
        setupProgress = SetupProgress::RegisterNetworkWait;
        disableDelay();
        break;
    }
    case SetupProgress::RegisterNetworkWait:
    {
        if (!LTE.isComplete())
            return;
        if (LTE.isSuccessful())
        {   
            Serial.println(F("[LTE] APN is registed successful"));
            setupProgress = SetupProgress::UNKNOWN;
        }
        else
        {
            setupProgress = SetupProgress::UNKNOWN;
        }
        break;
    }
    case SetupProgress::UNKNOWN:
        delayTimer.stop();
        break;

    default:

        break;
    }
}

void LTEManager::serialOnReceive(void *arg, String &payload)
{
    LTEManager &_this = *(LTEManager *)arg;

    //_this.serialLTE->println(payload);

    Serial.print("[SWS] : ");
    Serial.println(payload);
    //_this.LTE.onReceiveInvoke(payload);
}

void LTEManager::disableDelay()
{
    delayTimer.clearSettings();
    delayTimer.setInterval(0);
    delayTimer.start();
}