/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-05 00:37:38
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-05 15:16:30
 * @Description: 
 */
#include "OLED_lightweight.h"

void OLED::begin()
{
    // if (!display.begin(SSD1306_SWITCHCAPVCC))
    // {
    //     Serial.println(F("SSD1306 allocation failed"));
    //     for (;;)
    //         ; // Don't proceed, loop forever
    // }

    // display.drawPixel(10, 10, SSD1306_WHITE);

    // // Show initial display buffer contents on the screen --
    // // the library initializes this with an Adafruit splash screen.
    // display.display();
    start = millis();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    oled.begin(&Adafruit128x64, OLED_CS_PIN, OLED_DC_PIN, OLED_RESET_PIN);
    oled.setFont(Adafruit5x7);
    timer.start();
    showDeviceMenu();
}

void OLED::run()
{
    timer.run();
}

void OLED::showDeviceMenu()
{
    // Total time elapsed 38ms.
    oled.clear();
    oled.set2X();
    oled.println("ECIE LAB");
    oled.set1X();
    oled.print("LTE : ");
    oled.println("OFFLINE");
    oled.print("SD  : ");
    oled.println("OFFLINE");
    oled.print("MPPT  : ");
    oled.println("(0/6)");
    oled.print("BMS  : ");
    oled.println("ONLINE");
    oled.print("Elapsed  : ");
    oled.println(millis() - start);
}

void OLED::refreshCallback(SoftTimer &timer, void *arg)
{
    OLED *_this = (OLED *)arg;
    //_this->oled.
    _this->oled.clear();
    _this->oled.setCol(10);
    _this->oled.setRow(10);
    _this->oled.print("Elapsed  : ");
    _this->oled.println(mainPowerMonitor.getVoltage());
}