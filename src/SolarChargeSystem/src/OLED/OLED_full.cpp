/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-05 13:44:16
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-13 23:32:30
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
    oled.setCursor(0, 0);
    oled.setTextSize(1);              // Normal 1:1 pixel scale
    oled.setTextColor(SSD1306_WHITE); // Draw white text
    // oled.set2X();
    oled.println("ECIE LAB");
    // oled.set1X();
    printLTEStatus();
    oled.print("SD  : ");
    if (sdCardHelper.isReady())
        oled.println(sdCardHelper.getFileName());
    else
        oled.println("None");

    oled.print("MPPT  : ");
    //oled.println("(0/6)");
    oled.println(i2cManager.getDeviceCount());
    // oled.print("BMS  : ");
    //     oled.println("Unknown");

    oled.print("Elapsed  : ");
    //oled.println(millis());

    oled.print(hour());

    oled.print(":");
    oled.print(minute());

    oled.print(":");
    oled.println(second());

    start = millis();

    int totalRam = (int)RAMEND - (int)&__data_start;
    oled.print("SRAM :");
    oled.print(((float)mu_freeRam() / totalRam) * 100);
    oled.println(" %");
    oled.display();
}

void OLED::refreshCallback(SoftTimer &timer, void *arg)
{
    OLED *_this = (OLED *)arg;
    switch (_this->page)
    {
    case OLED_Page_Main:
        _this->showDeviceMenu();
        break;
    case OLED_Page_MPPT:
        _this->showMPPTMenu();

    default:
        break;
    }

    //_this->oled.
    // _this->display.clear();
    // _this->display.setCol(10);
    // _this->display.setRow(10);
    // _this->display.print("Elapsed  : ");
    // _this->display.println(mainPowerMonitor.getVoltage());
}

void OLED::printLTEStatus()
{
    oled.print("LTE : ");
#if defined(MODULE_LTE_EN)
    if (lteManager.isNetworkActive())
        oled.println(lteManager.getIPAddress());
    else if (!lteManager.isAvailabe())
        oled.println(F("Booting"));
    else
        oled.println(F("Setup Network"));
    
   
#elif
    oled.println("DISABLE")
#endif
}

void OLED::showMPPTMenu()
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.setTextSize(1);              // Normal 1:1 pixel scale
    oled.setTextColor(SSD1306_WHITE); // Draw white text

    oled.println("MPPT STATUS");
    oled.print("Data : ");
    oled.println(dataLogger.getDataCounts());
    //oled.println();
    oled.print("Vin  : ");
    oled.print(mpptModule.valTemp[0]);
    oled.println(" V");
    oled.print("Iin  : ");
    oled.print(mpptModule.valTemp[1]);
    oled.println(" A");
    oled.print("Vout : ");
    oled.print(mpptModule.valTemp[2]);
    oled.println(" V");
    oled.print("Iout : ");
    oled.print(mpptModule.valTemp[3]);
    oled.println(" A");

    oled.print("ILL : ");
    oled.print(envSensor.getValue(0));
    oled.println(" Lux");
    oled.print("IRR : ");
    oled.print(envSensor.getValue(0)*0.0079f);
    oled.println(" W/M2");
    
    oled.display();
}

void OLED::switchPage()
{
    if (page == OLED_Page_MPPT)
        page = OLED_Page_Main;
    else
        page = (Page)(page + 1);
    timer.reset();
    timer.forceExpired();
}