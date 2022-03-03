#ifndef __BATTERYBALANCE_H__
#define __BATTERYBALANCE_H__

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC68042.h"
#include <SPI.h>

#define First_detect_balance -1
#define Start_balance 0
#define Take_break_balance 1

static const char* batterybalance_header = "[BMS]";
#define BATTERYBALANCE_PRINTHEAD(x) Serial.print(batterybalance_header);

class BatteryBalance
{
public:
    BatteryBalance();
    void begin();
    void run();

private:
    /******* GLOBAL VARIABLES ***********/
    static constexpr uint8_t crc4_tab[16] = {
        // crc4_table
        0x0,
        0x7,
        0xe,
        0x9,
        0xb,
        0xc,
        0x5,
        0x2,
        0x1,
        0x6,
        0xf,
        0x8,
        0xa,
        0xd,
        0x4,
        0x3,
    };
    static const int CELL_NUM = 12;     //每一個IC上有幾顆電池
    static const int TOTAL_IC = 1;      // IC的數量
    uint8_t tx_cfg[TOTAL_IC][6]; // LTC6804-2的暫存器
    //***************voltage***********************
    uint16_t cell_codes[TOTAL_IC][CELL_NUM] = {(uint16_t)(4.0 * 10000), 
                                                (uint16_t)(3.9 * 10000), 
                                                (uint16_t)(3.6 * 10000), 
                                                (uint16_t)(4.15 * 10000), 
                                                (uint16_t)(3.6 * 10000), 
                                                4 * 10000, 
                                                0 * 10000, 
                                                0 * 10000, 
                                                0 * 10000, 
                                                0 * 10000, 
                                                0 * 10000, 
                                                0 * 10000}; //電池的各個電壓
    uint16_t GPIO_codes[TOTAL_IC][6];                                                                                                                                                           // GPIO的電壓訊息
    //***********************************************
    float all_voltage = 0.0;                                      //總電壓
    float avage_voltage = 0.0;                                    //平均電壓
    uint8_t active_discharge_flag[TOTAL_IC][12] = {0};            //主動充放電旗標
    uint8_t error;                                                //錯誤碼
    int balance = -2;
    int time_counter = 0, read_count = 0;
    int first = 0;
    float num_cell = 0.0;
    float high_vlotage = 0.0, low_vlotage = 5.0;
    int start_balance = 120, stop_balance = 2200;
    //******************************************************************************************
    uint8_t tx_comm_1[6]; //主動平衡暫存器-1
    uint16_t CRC_1 = 0;   // CRC碼

    unsigned long checkLastMillis = 0;
    unsigned int checkInterval = 1000;
    unsigned int checkInterval2 = 1100;

    void setupLTC6804();

    void init_cfg();
    void print_cells();
    void print_GPIO();
    void init_comm_1();
    void wrcomm_cofig();
    void crc();
    void HIGH_VOLTAGE_VS_LOW_VOLTAGE();
    void write_in_3300();
    uint8_t crc_new_logisim(uint16_t x);
    void detect_charge_discharge();
    void start_balance_end_condition();
    void end_condition();
    void print_information ();
    void read_voltage();
    void write_LTC3300();
    void excute_LTC3300();
    void interrupt_LTC3300();
    void polling_read ();
    void start_charge_discharge_function();
    void take_a_break_funcion();





};

#endif