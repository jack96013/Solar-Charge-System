/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-26 20:25:38
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-05 13:58:34
 * @Description: 
 */
#ifndef __OLED_H__
#define __OLED_H__

#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)
#include "OLED_lightweight.h"    
#elif defined(ARDUINO_ARCH_STM32F1)
#include "OLED_full.h"   
#endif

#include "DataLogger.h"

#endif // __OLED_H__