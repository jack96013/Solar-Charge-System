
/*
 * @ FileName		: main.cpp
 * @ Author			: TZU-CHIEH,HSU
 * @ Modified by	: TZU-CHIEH,HSU
 * @ Create Time	: 2021-12-04 14:57:43
 * @ Modified time	: 2021-12-11 15:54:47
 * @ Description	:   ECIE 110 Project
 *    
 *                      Solar Panel and Battery Manage System.
 *                      Program Entry Point.
 *                      Use State Machine Method and Event Driven to
 *                      implement multi tasking.
 *                      Don't use any delay() function in main loop.
 */

#include <Arduino.h>
#include <SPI.h>
#include "SerialManager.h"
#include "BatteryBalance/BatteryBalance.h"
#include "SoftTimer.h"
#include "LTEManager.h"
#include "SDCardHelper.h"
#include "Wire.h"
#include "OLED\OLED.h"
#include "MainPowerMonitor.h"
#include "usbDtrReset.h"
#include "DataLogger\DataLogger.h"
#include "MPPTModule\MPPTModule.h"
#include "ButtonHelper.h"
#include "EnvSensor.h"
#include "I2CManager.h"

SerialManager serialManager; // Serial 相關

DataLogger dataLogger;
MPPTModule mpptModule;

#ifdef MODULE_BMS_EN
BatteryBalance batteryBalance;
#endif
#ifdef MODULE_SD_EN
SDCardHelper sdCardHelper;
#endif
#ifdef MODULE_LTE_EN
LTEManager lteManager(serialManager);
#endif

MainPowerMonitor mainPowerMonitor;

// 代碼測試用
void testModuleBegin();
void testModuleRun();
void onExpiredCallback(SoftTimer &timer, void *arg);

SoftTimer sTimer1(20, onExpiredCallback, nullptr, SOFTTIMER_INFINITY);

unsigned long testMillis;
OLED oled;

bool button_last_state = 0;

EnvSensor envSensor;

#ifdef MODULE_I2CM_EN
I2CManager i2cManager;
#endif

ButtonHelper button1;

void setup()
{

    serialManager.begin();

#ifdef MODULE_SD_EN
    sdCardHelper.begin();
#endif
#ifdef MODULE_LTE_EN
    lteManager.begin();
#endif

    testModuleBegin();

    oled.begin();

    mainPowerMonitor.begin();
    dataLogger.begin();

    mpptModule.begin();
    envSensor.begin();
#ifdef MODULE_I2CM_EN
    i2cManager.begin();
#endif

#ifdef MODULE_BMS_EN
    batteryBalance.begin(); // 電池平衡
#endif

    // pinMode(A3, INPUT_PULLUP);
    button1.begin(A3,BUTTON_INPUT_PULLUP);
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

    oled.run();
#ifdef MODULE_LTE_EN
    lteManager.run();
#endif
#ifdef MODULE_BMS_EN

    //batteryBalance.run(); // 電池平衡
#endif
#ifdef MODULE_SD_EN
    sdCardHelper.run();
#endif
    dataLogger.run();

    testModuleRun();

    //mainPowerMonitor.run();

    mpptModule.run();
    envSensor.run();
    // ticks++;
    // if (millis() - pc_millis >= 1000)
    // {
    //     pc_millis = millis();
    //     DebugSerial.print("Ticks : ");
    //     DebugSerial.println(ticks);
    //     DebugSerial.print("t/s , Loading ");
    //     DebugSerial.print(100 - ticks / (float)max_ticks * 100);
    //     DebugSerial.println("%");

    //     ticks = 0;
    // }
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
    //sTimer1.run();
    button1.check();
    if (!button1.ready())
        return;
    if (button1.isShortPressed())
        oled.switchPage();
    else if (button1.isLongPressed())
    {
        Serial.println(F("Cali ADC"));
        mpptModule.calibrate();
    }
}

void onExpiredCallback(SoftTimer &timer, void *arg)
{

    bool state = !digitalRead(A3);
    if (state != button_last_state)
    {
        button_last_state = state;
        if (state == HIGH)
        {
            Serial.println(F("CALI"));
            oled.switchPage();
            //mpptModule.calibrate();
        }
    }
}
