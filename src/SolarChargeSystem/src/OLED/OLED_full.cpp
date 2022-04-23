/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-05 13:44:16
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-23 20:28:55
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
    //oled.setRotation(2);
    oled.drawPixel(10, 10, SSD1306_WHITE);

    // // Show initial display buffer contents on the screen --
    // // the library initializes this with an Adafruit splash screen.
    oled.display();

    start = millis();
    
    timer.start();
    showDeviceMenu();

    button1.begin(BUTTON1_PIN,BUTTON_INPUT_PULLUP);
    button2.begin(BUTTON2_PIN,BUTTON_INPUT_PULLUP);
}

void OLED::run()
{
    timer.run();
    
    Button1Handle();
    Button2Handle();
        
}

void OLED::showDeviceMenu()
{
    pageHeader("OVERALL");    

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
        break;
    case OLED_Page_Env:
        _this->showEnvMenu();
        break;
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

void OLED::Button1Handle()
{
    button1.check();
    if (!button1.ready())
        return;
    if (button1.isShortPressed())
    {
        switchPage();
    }
    else if (button1.isLongPressed())
        mpptModule.calibrate();
}

void OLED::Button2Handle()
{
    button2.check();
    if (!button2.ready())
        return;
    if (button2.isShortPressed())
    {
        if (page == OLED_Page_MPPT)
        {
            mpptModuleIndex ++;
            if (mpptModuleIndex == 6)
                mpptModuleIndex = 0;
            refreshRightNow();
        }
    }
}

void OLED::pageHeader(const char* header)
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.setTextSize(1);
    oled.fillRect(0,0,oled.width(),9,SSD1306_WHITE);
    oled.setTextColor(SSD1306_BLACK);
    oled.setCursor(1, 1);
    oled.println(header);
    oled.setTextColor(SSD1306_WHITE); 
}


void OLED::showMPPTMenu()
{
    pageHeader("MPPT Status");    
    
    oled.print("ID : ");
    oled.println(mpptModuleIndex);
    //oled.println();
    oled.print("Vin  : ");
    oled.print(mpptModule.getInputVoltage(mpptModuleIndex),3);
    oled.println(" V");
    oled.print("Iin  : ");
    oled.print(mpptModule.getInputCurrent(mpptModuleIndex),3);
    oled.println(" A");
    oled.print("Vout : ");
    oled.print(mpptModule.getOutputVoltage(mpptModuleIndex),3);
    oled.println(" V");
    oled.print("Iout : ");
    oled.print(mpptModule.getOutputCurrent(mpptModuleIndex),3);
    oled.println(" A");

    oled.print("ILL : ");
    oled.print(envSensor.getValue(0));
    oled.println(" Lux");
    oled.print("IRR : ");
    oled.print(envSensor.getValue(0)*0.0079f);
    oled.println(" W/M2");
    
    oled.display();
}


void OLED::showEnvMenu()
{
    pageHeader("Environment");   

    oled.println();
    oled.print("ILL : ");
    oled.print(envSensor.getValue(0));
    oled.println(" Lux");
    oled.print("IRR : ");
    oled.print(envSensor.getValue(0)*0.0079f);
    oled.println(" W/M2");
    oled.println();
    oled.print("Temp : ");
    oled.print(envSensor.getTemperature());
    oled.println(" C");
    oled.print("H : ");
    oled.print(0);
    oled.println(" M");
    oled.display();
}

void OLED::switchPage()
{
    if (page == MAX-1)
        page = (Page)0;
    else
        page = (Page)(page + 1);
    refreshRightNow();
}

void OLED::refreshRightNow()
{
    timer.reset();
    timer.forceExpired();
}