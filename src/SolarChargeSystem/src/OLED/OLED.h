/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-26 20:25:38
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-20 14:34:05
 * @Description: 
 */
#ifndef __OLED_H__
#define __OLED_H__

#include <Arduino.h>

#if defined(__AVR_ATmega328P__)
#include "OLED_lightweight.h"
#elif defined(__AVR_ATmega2560__)
#include "OLED_full.h"
#elif defined(ARDUINO_ARCH_STM32F1)
#include "OLED_full.h"   
#endif



#endif // __OLED_H__