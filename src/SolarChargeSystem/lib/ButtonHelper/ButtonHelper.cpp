

#include "ButtonHelper.h"

ButtonHelper::ButtonHelper()
{
    
}

void ButtonHelper::begin(int pin)
{
    begin(pin,BUTTON_INPUT_NORMAL);
}

void ButtonHelper::begin(int pin,byte input_mode)
{
    
    switch (input_mode)
    {
    case BUTTON_INPUT_NORMAL:
        pinMode(pin,INPUT);
        inverse = false;
        break;
    case BUTTON_INPUT_INVERSE:
        pinMode(pin,INPUT);
        inverse = true;
        break;
    case BUTTON_INPUT_PULLUP:
        pinMode(pin,INPUT_PULLUP);
        inverse = true;
    }
    ButtonHelper::pin = pin;
}

void ButtonHelper::read()
{
    _read(read_timeout);
}

void ButtonHelper::read(int16_t ms)
{
    _read(ms == -1 ? -1 :timeToCount(ms));
}

void ButtonHelper::_read(int16_t timeout_count)
{
    int16_t read_timeout_count = 0;
    check_last_millis = millis()-check_interval-10;
    do
    {
        check();
        if (read_finish)
        {
            if (timeout_count !=-1)
            {
                read_timeout_count ++;
                if (read_timeout_count-1 >= timeout_count)
                {
                    break;
                }
            }
        }
    } while (!ready());
}



/*----------------------------------------------------------------------*
/ Non-Blocking method to read button statue, if read complete ,use ready()
to check
/-----------------------------------------------------------------------*/
void ButtonHelper::check()
{
    if (millis()-check_last_millis>=check_interval)
    {
        check_last_millis=millis();
        buttonHandle();
        //Serial.println(result_code);
    }
    else
    {
        read_finish=false;
    }
}

void ButtonHelper::checkRightNow()
{
    buttonHandle();
}

void ButtonHelper::buttonHandle()
{
    //Process old statue
    if (result_code==BUTTON_PRESSED||result_code==BUTTON_LONG)
    {
        if (pressed_count < UINT16_MAX)
            pressed_count ++;
        if (pressed_count >= long_pressed_threshold)
        {
            double_click_count=0;
            result_code=BUTTON_LONG;
            if (pressed_count==long_pressed_threshold && long_pressed_callback != nullptr)
                long_pressed_callback(this);
            if (long_pressing_callback != nullptr)
                long_pressing_callback(this);
        } 
    }
    if (double_click_count!=0)
    {
        double_click_count++;
        if (double_click_count-1>doule_click_threshold)
            double_click_count=0;
    }
    if (double_clicked)
        double_clicked = false;

    //Process new statue, if result_code (old state) is BUTTON_SHORT,Force Release
    bool pin_status = inverse ? !digitalRead(pin):digitalRead(pin);
    if (pin_status && result_code != BUTTON_SHORT) //Pressed
    {
        if (result_code == BUTTON_RELEASE)
        {
            result_code = BUTTON_PRESSED;
            pressed_count=0;
            if (pressed_callback != nullptr)
                pressed_callback(this);
        }
    }
    else //Release
    {
        if (result_code == BUTTON_PRESSED)
        {
            //Short pressed occurs;
            result_code = BUTTON_SHORT; 
            
            if (short_pressed_callback != nullptr)
                short_pressed_callback(this);
            if (double_click_count==0)
                double_click_count = 1;
            else if (double_click_count -1 <= doule_click_threshold)
            {
                // Serial.println("double click !!!!");
                // Serial.print(">");    
                // Serial.println((double_click_count-1)*check_interval);
                double_clicked=true;
                double_click_count = 0;
                if (double_clicked_callback != nullptr)
                    double_clicked_callback(this);
            }
        }
        else if (result_code != BUTTON_RELEASE)
        {
            result_code = BUTTON_RELEASE;
            if (release_callback != nullptr)
                release_callback(this);
        }   
    }
    read_finish=true;
    if (ready_callback != nullptr)
    {
        ready_callback(this);
    }
}

bool ButtonHelper::ready()
{
    // if (read_finish && (result_code == BUTTON_LONG || result_code == BUTTON_SHORT))
    //     return true;
    // else
    //     return false;
    return read_finish;
}
/*
    Detect the button statue
*/
bool ButtonHelper::isPressed()
{
    return result_code>0;
}
bool ButtonHelper::isRelease()
{
    return result_code == 0;
}
bool ButtonHelper::isLongPressed()
{
    return (result_code == BUTTON_LONG) && (pressed_count == long_pressed_threshold);
    
}
bool ButtonHelper::isLongPressing()
{
    return result_code == BUTTON_LONG;
}
bool ButtonHelper::isShortPressed()
{
    return result_code == BUTTON_SHORT;
}
bool ButtonHelper::isDoubleClicked()
{
    return double_clicked;
}



void ButtonHelper::setThreshold(int threshold_ms)
{
    long_pressed_threshold = timeToCount(threshold_ms);
}
int ButtonHelper::getThreshold()
{
    return check_interval * long_pressed_threshold;
}

int ButtonHelper::getPin()
{
    return pin;
}

byte ButtonHelper::getStatueCode()
{
    return result_code;
}



void ButtonHelper::setScanInterval(unsigned int interval)
{
    int ms = check_interval * long_pressed_threshold;
    check_interval = interval;
    setThreshold(ms);
}

unsigned int ButtonHelper::getScanInterval()
{
    return check_interval;
}
void ButtonHelper::setReadTimeout(int16_t ms)
{
    read_timeout = timeToCount(ms);
}
int16_t ButtonHelper::getReadTimeout()
{
    return read_timeout*check_interval;
}
// Callback
void ButtonHelper::setReadyCallback(ButtonCallback func)
{
    ready_callback = func;
}
void ButtonHelper::setPressedCallback(ButtonCallback func)
{
    pressed_callback = func;
}
void ButtonHelper::setReleaseCallback(ButtonCallback func)
{
    release_callback = func;
}
void ButtonHelper::setLongPressedCallback(ButtonCallback func)
{
    long_pressed_callback = func;
}
void ButtonHelper::setLongPressingCallback(ButtonCallback func)
{
    long_pressing_callback = func;
}
void ButtonHelper::setShortPressedCallback(ButtonCallback func)
{
    short_pressed_callback = func;
}
void ButtonHelper::setDoubleClickedCallback(ButtonCallback func)
{
    double_clicked_callback = func;
}
uint16_t ButtonHelper::timeToCount(uint16_t ms)
{
    float temp = ms / (float)check_interval;
    return (int)((temp * 10) + 5) / 10.0;
}