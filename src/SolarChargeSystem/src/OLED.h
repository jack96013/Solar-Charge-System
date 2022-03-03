#ifndef __OLED_H__
#define __OLED_H__
#include <Arduino.h>

// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

#include "SSD1306Ascii.h"
#include "SSD1306AsciiSpi.h"

#include "config/config.h"

#define OLED_WIDTH 128 // OLED display width, in pixels
#define OLED_HEIGHT 64 // OLED display height, in pixels


// D0 SCK / SCL     
// D1 MOSI / SDA    
// RES              D3
// DC DATACOMMAND   D4
// CS               D5

#define OLED_RESET_PIN  3
#define OLED_DC_PIN     4
#define OLED_CS_PIN     5

#include <Arduino.h>
#include "SoftTimer.h"
#include "MainPowerMonitor.h"

extern MainPowerMonitor mainPowerMonitor;

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
        
        static void refreshCallback(SoftTimer * ,void*);
        SoftTimer timer = SoftTimer(1000,refreshCallback,this,SOFTTIMER_INFINITY);
};

#endif // __OLED_H__