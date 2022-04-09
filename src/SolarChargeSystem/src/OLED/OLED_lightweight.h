/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-05 00:37:11
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-27 20:29:43
 * @Description: 
 */


#ifndef __OLED_LIGHTWEIGHT_H__
#define __OLED_LIGHTWEIGHT_H__

#include <Arduino.h>
#include "SoftTimer.h"
#include "MainPowerMonitor.h"
#include "SSD1306AsciiSpi.h"
#include "SDCardHelper.h"
#include "DataLogger.h"
#include "config/Config.h"
#include "LightSensor.h"

extern MainPowerMonitor mainPowerMonitor;
extern SDCardHelper sdCardHelper;
extern DataLogger dataLogger;
extern LightSensor lightSensor;
class OLED
{

    public:
        void begin();
        void run();
        void showDeviceMenu();

    private:
        
        SSD1306AsciiSpi oled;
        uint32_t start;
        // Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT,
        // &SPI, OLED_DC_PIN, OLED_RESET_PIN, OLED_CS_PIN);
        
        static void refreshCallback(SoftTimer& ,void*);
        SoftTimer timer = SoftTimer(1000,refreshCallback,this,SOFTTIMER_INFINITY);
};

#endif // __OLED_LIGHTWEIGHT_H__
