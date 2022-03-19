// Arduino Button Library
// https://github.com/jack96013
// Copyright (C) 2020 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch demonstrating short and long button presses.
//
//

#ifndef BUTTON_HELPER_H_
#define BUTTON_HELPER_H_
#include <Arduino.h>
#define BUTTON_RELEASE 0
#define BUTTON_PRESSED 1
#define BUTTON_SHORT 2
#define BUTTON_LONG 3
#define BUTTON_COUNT_READY 0
#define BUTTON_COUNT_FINISH 1

#define BUTTON_INPUT_NORMAL 0
#define BUTTON_INPUT_INVERSE 1
#define BUTTON_INPUT_PULLUP 2


class ButtonHelper
{
    public:
        typedef void (*ButtonCallback)(ButtonHelper*);

        ButtonHelper();
        void startRead();
        
        void begin(int);
        void begin(int,byte);
        
        void setScanInterval(unsigned int); //掃描間隔
        unsigned int getScanInterval(); //掃描間隔


        void read(); //Check button with blocking
        void read(int16_t); 
        void setReadTimeout(int16_t);
        int16_t getReadTimeout();

        void check(); //Check button with non-blocking
        void checkRightNow();
        
        void setThreshold(int);
        int getThreshold();

        void setPin();
        int getPin();

        void setInverse(bool);
        void getInverse();

        byte getStatueCode();

        bool ready();
        bool isPressed();
        bool isRelease();
        bool isLongPressed(); //Button still long pressed , but it occured one interval
        bool isLongPressing(); //Button still long pressed , but it occured untill release
        bool isShortPressed();
        bool isDoubleClicked();

        void setReadyCallback(ButtonCallback);
        void setPressedCallback(ButtonCallback);
        void setReleaseCallback(ButtonCallback);
        void setLongPressedCallback(ButtonCallback);
        void setLongPressingCallback(ButtonCallback);
        void setShortPressedCallback(ButtonCallback);
        void setDoubleClickedCallback(ButtonCallback);


    private:
        void buttonHandle();
        uint16_t timeToCount(uint16_t ms);
        void _read(int16_t ms);
        byte pin;

        unsigned long check_last_millis=0;
        unsigned int check_interval = 20;
        int16_t read_timeout = 25;

        uint16_t pressed_count; 
        
        uint8_t long_pressed_threshold=15; // 10=300/20
        uint8_t double_click_count=0;
        uint8_t doule_click_threshold=18;
        bool double_clicked = false;

        byte result_code = 0;
        bool inverse = false;
   
        bool read_finish=false;

        ButtonCallback ready_callback=nullptr;
        ButtonCallback pressed_callback=nullptr;
        ButtonCallback release_callback=nullptr;
        ButtonCallback long_pressed_callback=nullptr;
        ButtonCallback long_pressing_callback=nullptr;
        ButtonCallback short_pressed_callback=nullptr;
        ButtonCallback double_clicked_callback=nullptr;

};


#endif