/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-05 13:44:06
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-23 20:28:36
 * @Description: 
 */


#ifndef __OLED_FULL_H__
#define __OLED_FULL_H__

#include <Arduino.h>
#include <SoftTimer.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include <MemoryUsage.h>
#include <ButtonHelper.h>

#include "config/Config.h"
#include "SDCardHelper.h"
#include "DataLogger/DataLogger.h"



extern SDCardHelper sdCardHelper;
extern DataLogger dataLogger;


class OLED
{

    public:
        void begin();
        void run();
        void showDeviceMenu();
        void showMPPTMenu();
        void showEnvMenu();
        void switchPage();
        void refreshRightNow();
        
        // LTE
        
        

    private:
    
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
            OLED_Page_MPPT,
            OLED_Page_Env,
            MAX
        };
        
        Page page = OLED_Page_Main;

        void printLTEStatus();

        ButtonHelper button1;
        ButtonHelper button2;
        
        void Button1Handle();
        void Button2Handle();
        void pageHeader(const char* header);
        void printHeader(const __FlashStringHelper *ifsh);

        int mpptModuleIndex = 0;
};

#endif // __OLED_FULL_H__