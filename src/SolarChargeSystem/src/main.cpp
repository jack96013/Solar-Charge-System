/*
 * @ FileName		: main.cpp
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-04 14:57:43
 * @ Modified time	: 2021-12-11 15:54:47
 * @ Description	: Program Entry Point.
 */

#include <Arduino.h>
#include "SerialManager.h"
#include "BatteryBalance/BatteryBalance.h"
#include "SoftTimer.h"
#include "LTEManager.h"
#include "SDCardHelper.h"
#include "Wire.h"
#include "OLED.h"
#include "MainPowerMonitor.h"

SerialManager serialManager; // Serial 相關
BatteryBalance batteryBalance;
LTEManager lte(serialManager);
//SDCardHelper sdCardHelper;
OLED oled;
MainPowerMonitor mainPowerMonitor;

// 代碼測試用
void testModuleBegin();
void testModuleRun();
void onExpiredCallback(SoftTimer &timer, void *arg);

SoftTimer sTimer1(3000, onExpiredCallback, nullptr, 3);

unsigned long testMillis;

void setup()
{

    serialManager.begin();
    //sdCardHelper.begin();

    batteryBalance.begin(); // 電池平衡
    //testModuleBegin();
    lte.begin();
    oled.begin();

    mainPowerMonitor.begin();
}

uint16_t ticks = 0;
uint32_t pc_millis = 0;
uint32_t pc_report_millis = 0;
uint32_t max_ticks = 27404;

void loop()
{
    testMillis = millis();
    serialManager.run();
    batteryBalance.run();
    lte.run();
    testModuleRun();

    mainPowerMonitor.run();
    oled.run();
    ticks ++;
    if (millis()-pc_millis >= 1000)
    {
        pc_millis = millis();
        Serial.print("Ticks : ");
        Serial.print(ticks);
        Serial.print("t/s , Loading ");
        Serial.print(100 - ticks / (float)max_ticks * 100);
        Serial.println("%");
        
        ticks = 0;
    }
    //unsigned long elapsed = millis()-testMillis;
    // if (elapsed>1)
    //     Serial.println(elapsed);
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