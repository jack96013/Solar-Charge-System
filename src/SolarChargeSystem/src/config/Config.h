/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2021-12-04 15:11:45
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-23 17:10:33
 * @Description:  雙平台，Arduino & STM32
 */


#define __CONFIG_H__
#include <Arduino.h>



#if defined(__AVR_ATmega328P__) 
#include "BoardConfig_328P.h" 
#elif defined(__AVR_ATmega2560__)
#include "BoardConfig_2560.h"
#elif defined(ARDUINO_ARCH_STM32F1)
#include "BoardConfig_STM.h"
#endif


// Serial
#define SERIAL_BAUD 115200
#define SERIAL_BUFFER_LEN 100


// SD Card

#define SD_FILE_BASENAME "log-"
#define SD_FILE_TYPE "csv"


// MPPT MODULE (舊版 Layout)
// #define MPPT_DEVICES 6
// #define MPPT_ADDRESS_START 64
// #define MPPT_VIN_GAIN  4.0f
// #define MPPT_VOUT_GAIN 2.20f   //2.204
// #define MPPT_I_GAIN_SEN 2.2222222f    
// #define MPPT_I_GAIN_DIV 1.576f
// #define MPPT_I_ZCO      2.52

// MPPT MODULE (新版 Layout)
#define MPPT_DEVICES 6
#define MPPT_ADDRESS_START 64
#define MPPT_VIN_GAIN  4.3f
#define MPPT_VOUT_GAIN 2.20f   //2.204
#define MPPT_I_GAIN_SEN -1.0f    
#define MPPT_I_GAIN_DIV 2.0f
#define MPPT_I_ZCO      2.5
// ADC 編號
#define MPPT_VIN_INDEX 0    
#define MPPT_IIN_INDEX 1
#define MPPT_VOUT_INDEX 3
#define MPPT_IOUT_INDEX 2

// CQ330G 
// ZCO = 2.50V
// SEN = 450mV/A
// GAIN_SEN = 1/SEN
// GAIN_DIV = (10k + 4.7k) / 10k = 1.47

// I = (1/GAIN_DIV-ZCO)*GAIN_SEN = 

#define LTE_MQTT_HOST ""
#define LTE_MQTT_USERNAME ""
#define LTE_MQTT_PASSWORD ""

#define ENV_BMP180_EN
#define ENV_BMP180_ADDR


