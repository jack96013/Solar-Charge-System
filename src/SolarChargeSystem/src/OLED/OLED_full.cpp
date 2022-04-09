/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-05 13:44:16
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-27 20:33:18
 * @Description: 
 */

#include "OLED_full.h"

void OLED::begin()
{
    if (!oled.begin(SSD1306_SWITCHCAPVCC))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    oled.drawPixel(10, 10, SSD1306_WHITE);

    // // Show initial display buffer contents on the screen --
    // // the library initializes this with an Adafruit splash screen.
    oled.display();

    start = millis();
    //SPI.setClockDivider(SPI_CLOCK_DIV2);
    // oled.begin(&Adafruit128x64, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN);
    // oled.setFont(Adafruit5x7);
    timer.start();
    showDeviceMenu();
}

void OLED::run()
{
    timer.run();
}

void OLED::showDeviceMenu()
{
    //Total time elapsed 38ms.
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.setTextSize(1);             // Normal 1:1 pixel scale
    oled.setTextColor(SSD1306_WHITE);        // Draw white text
    // oled.set2X();
    oled.println("ECIE LAB");
    // oled.set1X();
    oled.print("LTE : ");
    oled.println("OFFLINE");
    oled.print("SD  : ");
    if (sdCardHelper.isReady())
        oled.println("INSERTED");
    else
        oled.println("None");
    
    oled.print("MPPT  : ");
    //oled.println("(0/6)");
    oled.println(dataLogger.getDataCounts());
    oled.print("BMS  : ");
        oled.println("Unknown");
    
    oled.print("Elapsed  : ");
    oled.println(millis()-start);  
    start = millis();
    oled.display();
    
}

void OLED::refreshCallback(SoftTimer &timer ,void* arg)
{
    OLED* _this = (OLED*) arg;
    _this->showDeviceMenu();
    

    //_this->oled.
    // _this->display.clear();
    // _this->display.setCol(10);
    // _this->display.setRow(10);
    // _this->display.print("Elapsed  : ");
    // _this->display.println(mainPowerMonitor.getVoltage());
}
