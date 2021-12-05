#include <Arduino.h>
#include "SerialManager.h"
#include "BatteryBalance.h"
#include "SoftTimer.h"

SerialManager serialManager; // Serial 相關
BatteryBalance batteryBalance;

// 代碼測試用
void testModuleBegin();
void testModuleRun();
void onExpiredCallback(SoftTimer& timer,void* arg);

SoftTimer sTimer1(3000,onExpiredCallback,nullptr,3);


void setup()
{
    serialManager.begin();
    batteryBalance.begin(); // 電池平衡
    testModuleBegin();
}

void loop()
{
    serialManager.run();
    batteryBalance.run();
    testModuleRun();
}

void testModuleBegin()
{
    sTimer1.start();

}
void testModuleRun()
{   
    sTimer1.run();
}

void onExpiredCallback(SoftTimer& timer,void* arg)
{
    Serial.println("good");
    
}