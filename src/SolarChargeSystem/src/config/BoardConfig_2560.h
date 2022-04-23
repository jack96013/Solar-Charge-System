/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-03-04 22:54:11
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-19 19:01:20
 * @Description: 
 */

// Debug
#define DebugSerial Serial


#define GLOBAL_SPI_DIVIDE SPI_CLOCK_DIV2   // 全域 SPI 設定
#define BMS_SPI_DIVIDE SPI_CLOCK_DIV16  // 16M / 16 = 1M SPI (LTC6804 Maximum)

// Module Enable
#define MODULE_BMS_EN
#define MODULE_SD_EN
#define MODULE_LTE_EN
#define MODULE_I2CM_EN

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
#define MAINPWR_MON_HALL_GAIN        80  // 20A/0.625=32

#define MAINPWR_MON_V_PIN            A2
#define MAINPWR_MON_V_GAIN           1.0f    

// SD CARD
#define SD_SPI
#define SD_CS_PIN 12

// MPPT MODULE
#define MPPT_I2C

// // OLED
// #define OLED_RESET_PIN  3
// #define OLED_DC_PIN     4
// #define OLED_CS_PIN     5

#define OLED_RESET_PIN  2
#define OLED_DC_PIN     3
#define OLED_CS_PIN     5

#define BUTTON1_PIN 4
#define BUTTON2_PIN 6

