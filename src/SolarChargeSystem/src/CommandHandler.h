/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-04-24 04:00:17
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-24 04:06:32
 * @Description: 
 */
#ifndef __COMMANDHANDLER_H__
#define __COMMANDHANDLER_H__
#include <Arduino.h>
#include "BatteryBalance/BatteryBalance.h"

class CommandHandler
{
    public:
    CommandHandler();
    void begin();
    void run();
    
    void executeCommand(String &payload);
    
};


#endif // __COMMANDHANDLER_H__