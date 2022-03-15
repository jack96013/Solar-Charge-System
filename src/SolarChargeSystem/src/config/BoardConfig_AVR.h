/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-04 22:54:11
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-15 00:51:27
 * @Description: 
 */

// Debug
#define DebugSerial Serial

// Module Enable
//#define MODULE_BMS_EN
#define MODULE_SD_EN

// LTE 
#define LTE_SERIAL AltSoftSerial
#define LTE_SERIAL_BAUD 57600 
#define LTE_SERIAL_RX 2
#define LTE_SERIAL_TX 3
#define LTE_SERIAL_BUFFER_LEN 100

// MAIN POWER MONITOR
#define MAINPWR_MON_VREF    5.0f
#define MAINPWR_MON_HALL_PIN         A1
#define MAINPWR_MON_HALL_REF_PIN     A0    
#define MAINPWR_MON_HALL_GAIN        32  // 20A/0.625=32

#define MAINPWR_MON_V_PIN            A2
#define MAINPWR_MON_V_GAIN           1.0f    

// SD CARD
#define SD_SPI
#define SD_CS_PIN 6

// MPPT MODULE
#define MPPT_I2C

