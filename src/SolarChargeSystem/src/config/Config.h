/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2021-12-04 15:11:45
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-15 00:51:22
 * @Description:  雙平台，Arduino & STM32
 */


#define __CONFIG_H__
#include <Arduino.h>



#if defined(ARDUINO_ARCH_AVR) 
#include "BoardConfig_AVR.h" 
#elif defined(ARDUINO_ARCH_STM32F1)
#include "BoardConfig_STM.h"
#endif



// Serial
#define SERIAL_BAUD 115200
#define SERIAL_BUFFER_LEN 100

// MPPT MODULE
#define MPPT_IIN_PIN    6
#define MPPT_IIN_GAIN   6
#define MPPT_VIN_PIN    6
#define MPPT_VIN_GAIN   6

#define MPPT_IOUT_PIN    6
#define MPPT_IOUT_GAIN   6
#define MPPT_VOUT_PIN    6
#define MPPT_VOUT_GAIN   6

// SD Card

#define SD_FILE_BASENAME "log-"
#define SD_FILE_TYPE "csv"


#define MPPT_DEVICES 6
#define MPPT_ADDRESS_START 64
#define MPPT_VIN_GAIN  4.0f
// 
#define MPPT_VOUT_GAIN 2.20f   //2.204
#define MPPT_I_GAIN_SEN 2.2222222f    
#define MPPT_I_GAIN_DIV 1.576f
#define MPPT_I_ZCO      2.52
// CQ330G 
// ZCO = 2.50V
// SEN = 450mV/A
// GAIN_SEN = 1/SEN
// GAIN_DIV = (10k + 4.7k) / 10k = 1.47

// I = (1/GAIN_DIV-ZCO)*GAIN_SEN = 

