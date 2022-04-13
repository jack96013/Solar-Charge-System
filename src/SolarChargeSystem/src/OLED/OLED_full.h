/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-05 13:44:06
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-27 20:31:25
 * @Description: 
 */


#ifndef __OLED_FULL_H__
#define __OLED_FULL_H__

#include <Arduino.h>
#include <SoftTimer.h>

#include <Adafruit_SSD1306.h>
#include "config/Config.h"
#include "SDCardHelper.h"
#include "DataLogger/DataLogger.h"
#include <TimeLib.h>
#include <MemoryUsage.h>

extern SDCardHelper sdCardHelper;
extern DataLogger dataLogger;

#define OLED_RESET_PIN  3
#define OLED_DC_PIN     4
#define OLED_CS_PIN     5

class OLED
{

    public:
        void begin();
        void run();
        void showDeviceMenu();
        void showMPPTMenu();
        void switchPage();

    private:
                                //     Adafruit_SSD1306(uint8_t w, uint8_t h, SPIClass *spi,
                                //    int8_t dc_pin, int8_t rst_pin, int8_t cs_pin,
                                //    uint32_t bitrate)
        Adafruit_SSD1306 oled =  Adafruit_SSD1306((uint8_t)128, (uint8_t)64, &SPI, (int8_t)OLED_DC_PIN,
                   (int8_t)OLED_RESET_PIN, (int8_t)OLED_CS_PIN);
                  
                   
        uint32_t start;
        // Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT,
        //     &SPI, OLED_DC_PIN, OLED_RESET_PIN, OLED_CS_PIN);
        
        static void refreshCallback(SoftTimer& ,void*);
        SoftTimer timer = SoftTimer(1000,refreshCallback,this,SOFTTIMER_INFINITY);
        
        enum Page
        {
            OLED_Page_Main,
            OLED_Page_MPPT
        };
        
        Page page = OLED_Page_Main;
};

#endif // __OLED_FULL_H__