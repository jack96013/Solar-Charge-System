
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
#include "OLED/OLED.h"
#include "MainPowerMonitor.h"
#include "usbDtrReset.h"
#include "DataLogger.h"

SerialManager serialManager; // Serial 相關
LTEManager lte(serialManager);
DataLogger dataLogger;

#ifdef MODULE_BMS_EN
BatteryBalance batteryBalance;
#endif
#ifdef MODULE_SD_EN
SDCardHelper sdCardHelper;
#endif

//OLED oled;
MainPowerMonitor mainPowerMonitor;

// 代碼測試用
void testModuleBegin();
void testModuleRun();
void onExpiredCallback(SoftTimer &timer, void *arg);

SoftTimer sTimer1(3000, onExpiredCallback, nullptr, 3);

unsigned long testMillis;
OLED oled;


void setup()
{

    serialManager.begin();

#ifdef MODULE_BMS_EN
    batteryBalance.begin(); // 電池平衡
#endif
#ifdef MODULE_SD_EN
    sdCardHelper.begin();
#endif
    dataLogger.begin();
    

    testModuleBegin();
    lte.begin();
    oled.begin();

    mainPowerMonitor.begin();
}

uint32_t ticks = 0;
uint32_t pc_millis = 0;
uint32_t pc_report_millis = 0;
//uint32_t max_ticks = 27404;
uint32_t max_ticks = 606756;

void loop()
{

    //testMillis = millis();
    serialManager.run();
    lte.run();
    oled.run();

#ifdef MODULE_BMS_EN
    batteryBalance.run(); // 電池平衡
#endif
#ifdef MODULE_SD_EN
    sdCardHelper.run();
#endif
    dataLogger.run();

    //testModuleRun();
    dtrResetRun();

    mainPowerMonitor.run();
    // //oled.run();
    ticks++;
    if (millis() - pc_millis >= 1000)
    {
        pc_millis = millis();
        DebugSerial.print("Ticks : ");
        DebugSerial.println(ticks);
        DebugSerial.print("t/s , Loading ");
        DebugSerial.print(100 - ticks / (float)max_ticks * 100);
        DebugSerial.println("%");

        ticks = 0;
    }
    // unsigned long elapsed = millis()-testMillis;
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