/*
 * @ FileName		: main.cpp
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-04 14:57:43
 * @ Modified time	: 2021-12-06 22:13:42
 * @ Description	: Program Entry Point.
 */

#include <Arduino.h>
#include "SerialManager.h"
#include "BatteryBalance.h"
#include "SoftTimer.h"
#include "LTEManager.h"

SerialManager serialManager; // Serial 相關
BatteryBalance batteryBalance;
LTEManager lte(serialManager);


// 代碼測試用
void testModuleBegin();
void testModuleRun();
void onExpiredCallback(SoftTimer &timer, void *arg);

SoftTimer sTimer1(3000, onExpiredCallback, nullptr, 3);



void setup()
{
    serialManager.begin();
    batteryBalance.begin(); // 電池平衡
    testModuleBegin();
    lte.begin();
    
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

void onExpiredCallback(SoftTimer &timer, void *arg)
{
    Serial.println("good");
}