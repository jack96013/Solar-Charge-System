
#include "BatteryBalance.h"

BatteryBalance::BatteryBalance()
{
}

void BatteryBalance::begin()
{
    //LTC6804_initialize(); // Initialize LTC6804 hardware
    setupLTC6804();
    init_cfg();           // Initialize the 6804 configuration array to be written
    init_comm_1();
    wakeup_sleep();
    LTC6804_clrcell();
    LTC6804_clraux();
    // delay(1000);
    // read_voltage();
    // delay(1000);
    // read_voltage();
}

void BatteryBalance::run()
{
    polling_read();
    // if (Serial.available())
    // {
    //     char val = Serial.read();
    //     switch (val)
    //     {
    //     case '1': //中斷3300
    //         interrupt_LTC3300();
    //         balance = 4;
    //         break;
    //     case '2': //開始
    //         balance = First_detect_balance;
    //         break;
    //     case '3': //讀電池電壓
    //         // read_voltage();
    //         break;
    //     }
    // }
    // if (balance == First_detect_balance)
    // {
    //     take_a_break_funcion();
    // }
    // else if (balance == Start_balance)
    // {
    //     for (int i = 0; i < TOTAL_IC; i++)
    //         for (int j = 0; j < CELL_NUM; j++)
    //             Serial.print(active_discharge_flag[i][j]);
    //     Serial.println();
    //     /* Serial.println(time_counter);
    //       Serial.println(start_balance);*/
    //     // Serial.println("執行30秒");
    //     start_charge_discharge_function();
    // }
    // else if (balance == Take_break_balance)
    // {
    //     Serial.println("暫停30秒");
    //     first = 0;
    //     delay(10);
    //     time_counter++;
    //     take_a_break_funcion();
    //     //***************************
    // }
}

void BatteryBalance::setupLTC6804()
{
    quikeval_SPI_connect();
    spi_enable(SPI_CLOCK_DIV16);
    set_adc(MD_NORMAL, DCP_DISABLED, CELL_CH_ALL, AUX_CH_ALL);

}

void BatteryBalance::init_cfg()
{
    for (int i = 0; i < TOTAL_IC; i++)
    {
        tx_cfg[i][0] = 0XFC;
        tx_cfg[i][1] = 0x00;
        tx_cfg[i][2] = 0x00;
        tx_cfg[i][3] = 0x00;
        tx_cfg[i][4] = 0x00;
        tx_cfg[i][5] = 0x10;
    }
}
//*****************************印出電池地壓****************************************************
void BatteryBalance::print_cells()
{
    for (int current_ic = 0; current_ic < TOTAL_IC; current_ic++)
    {
        Serial.print(" IC ");
        Serial.print(current_ic + 1, DEC);
        for (int i = 0; i < CELL_NUM; i++)
        {
            Serial.print(" C");
            Serial.print(i + 1, DEC);
            Serial.print(":");
            Serial.print(cell_codes[current_ic][i] * 0.0001, 4);
            Serial.print(",");
        }
        Serial.println();
    }
}
//***********************************印出GPIO電壓*************************************************
void BatteryBalance::print_GPIO()
{
    for (int current_ic = 0; current_ic < TOTAL_IC; current_ic++)
    {
        Serial.print(" IC ");
        Serial.print(current_ic + 1, DEC);
        for (int i = 0; i < 6; i++)
        {
            Serial.print(" GPIO");
            Serial.print(i + 1, DEC);
            Serial.print(":");
            Serial.print(GPIO_codes[current_ic][i] * 0.0001, 4);
            Serial.print(",");
        }
        Serial.println();
    }
}
//***************************主動平衡***********************************************************
void BatteryBalance::init_comm_1()
{ // LTC3300第一顆
    tx_comm_1[0] = 0X8A;
    tx_comm_1[1] = 0x98;
    tx_comm_1[2] = 0x8B;
    tx_comm_1[3] = 0xB8;
    tx_comm_1[4] = 0x80;
    tx_comm_1[5] = 0xF9;
}
//********************寫入主動暫存器****************************
void BatteryBalance::wrcomm_cofig(void)
{
    LTC6804_wrcomm(0, tx_comm_1);
    LTC6804_stcomm(0, 9);
}
//**************************crc************************************
void BatteryBalance::crc(void)
{
    CRC_1 = ((tx_comm_1[2] & 0X0F) << 8) + ((tx_comm_1[3] & 0XF0)) + (tx_comm_1[4] & 0X0F);
}
//*************************判斷主動充放電*********************************
void BatteryBalance::HIGH_VOLTAGE_VS_LOW_VOLTAGE(void)
{
    num_cell = 0.0;
    all_voltage = 0.0;
    avage_voltage = 0.0;
    for (int i = 0; i < TOTAL_IC; i++)
        for (int j = 0; j < CELL_NUM; j++)
            active_discharge_flag[i][j] = 0;
    polling_read();
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if ((float)cell_codes[i][j] * 0.0001 > 3.5 && (float)cell_codes[i][j] * 0.0001 < 4.1)
            {
                all_voltage = all_voltage + (float)cell_codes[i][j] * 0.0001;
                num_cell++;
            }
        }
    }
    avage_voltage = all_voltage / num_cell;
    polling_read();
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if ((float)cell_codes[i][j] * 0.0001 > avage_voltage && ((float)cell_codes[i][j] * 0.0001 <= 4.1))
                active_discharge_flag[i][j] = 2;
            else if ((float)cell_codes[i][j] * 0.0001 < avage_voltage && ((float)cell_codes[i][j] * 0.0001 >= 3.5))
                active_discharge_flag[i][j] = 3;
            else
                active_discharge_flag[i][j] = 0;
        }
    }
}
void BatteryBalance::write_in_3300(void)
{
    for (int j = 0; j < CELL_NUM; j++)
    {
        if (j == 0 || j == 1)
        {
            if (j == 0)
                tx_comm_1[2] = 0x80;
            tx_comm_1[2] = tx_comm_1[2] | active_discharge_flag[0][j] << (2 - 2 * j);
        }
        else if (j == 2 || j == 3)
        {
            if (j == 2)
                tx_comm_1[3] = 0x08;
            tx_comm_1[3] = tx_comm_1[3] | active_discharge_flag[0][j] << (10 - 2 * j);
        }
        else if (j == 4 || j == 5)
        {
            if (j == 4)
                tx_comm_1[4] = 0x80;
            tx_comm_1[4] = tx_comm_1[4] | active_discharge_flag[0][j] << (10 - 2 * j);
        }
    }
}
/**
 * @brief CRC15 Method
 * @author Orden
 * @param x 
 * @return uint8_t 
 */
uint8_t BatteryBalance::crc_new_logisim(uint16_t x)
{
    int crc[12];
    int crc_second[4];
    for (int i = 0; i < 12; i++)
    {
        crc[i] = (x & (0x01 << i)) >> i;
    }
    crc_second[3] = (!(((((crc[6] ^ crc[10]) ^ crc[9]) ^ crc[2]) ^ ((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8]))) ^ (((((crc[7] ^ crc[11]) ^ crc[10]) ^ crc[3]) ^ ((crc[6] ^ crc[10]) ^ crc[9])) ^ 0)));
    crc_second[2] = (!((((((crc[6] ^ crc[10]) ^ crc[9]) ^ crc[2]) ^ ((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8]))) ^ 0) ^ ((((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8])) ^ crc[1]) ^ (((crc[11] ^ crc[8]) ^ crc[4]) ^ ((crc[7] ^ crc[11]) ^ crc[10])))));
    crc_second[1] = (!((((((crc[11] ^ crc[8]) ^ crc[4]) ^ ((crc[7] ^ crc[11]) ^ crc[10])) ^ crc[0]) ^ ((((crc[7] ^ crc[11]) ^ crc[10]) ^ crc[3]) ^ ((crc[6] ^ crc[10]) ^ crc[9]))) ^ (((((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8])) ^ crc[1]) ^ (((crc[11] ^ crc[8]) ^ crc[4]) ^ ((crc[7] ^ crc[11]) ^ crc[10]))) ^ 0)));
    crc_second[0] = (!((((((crc[11] ^ crc[8]) ^ crc[4]) ^ ((crc[7] ^ crc[11]) ^ crc[10])) ^ crc[0]) ^ ((((crc[7] ^ crc[11]) ^ crc[10]) ^ crc[3]) ^ ((crc[6] ^ crc[10]) ^ crc[9]))) ^ 0));
    return (crc_second[3] << 3 | crc_second[2] << 2 | crc_second[1] << 1 | crc_second[0]);
}

void BatteryBalance::detect_charge_discharge(void)
{
    HIGH_VOLTAGE_VS_LOW_VOLTAGE();
    polling_read();
    write_in_3300();
    tx_comm_1[5] = 0X09;
    crc();
    tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim(CRC_1)) << 4);
}
//*********************************************************************************************
void BatteryBalance::start_balance_end_condition(void)
{
    if (time_counter >= start_balance) // 30s
    {
        interrupt_LTC3300();
        balance = 1;
        read_count = 0;
        time_counter = 0;
    }
}
/*********************************************************************************************/
void BatteryBalance::end_condition(void)
{
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if ((float)cell_codes[i][j] * 0.0001 >= high_vlotage && (float)cell_codes[i][j] * 0.0001 <= 4.1 && (float)cell_codes[i][j] * 0.0001 >= 3.5)
                high_vlotage = (float)cell_codes[i][j] * 0.0001;
            if ((float)cell_codes[i][j] * 0.0001 <= low_vlotage && (float)cell_codes[i][j] * 0.0001 >= 3.5 && (float)cell_codes[i][j] * 0.0001 <= 4.1)
                low_vlotage = (float)cell_codes[i][j] * 0.0001;
        }
    }
}
/******************************印出所有資訊***************************************************/
void BatteryBalance::print_information(void)
{
    for (int i = 0; i < TOTAL_IC; i++)
        for (int j = 0; j < CELL_NUM; j++)
            Serial.print(cell_codes[i][j] * 0.0001, 4);
}
void BatteryBalance::read_voltage(void)
{
    wakeup_idle();
    LTC6804_adcv();
    error = LTC6804_rdcv(0, TOTAL_IC, cell_codes);
    //print_information ();
}
void BatteryBalance::write_LTC3300(void)
{
    wakeup_idle();
    tx_comm_1[0] = 0X8A;
    tx_comm_1[1] = 0x98;
    wrcomm_cofig();
}
void BatteryBalance::excute_LTC3300(void)
{
    wakeup_idle();
    tx_comm_1[0] = 0X8A;
    tx_comm_1[1] = 0xF8;
    wrcomm_cofig();
}
void BatteryBalance::interrupt_LTC3300(void)
{
    wakeup_idle();
    tx_comm_1[0] = 0X8A;
    tx_comm_1[1] = 0xE8;
    wrcomm_cofig();
}
void BatteryBalance::polling_read(void)
{
    if (millis() - checkLastMillis >= checkInterval && millis() - checkLastMillis <= checkInterval2)
    {
        SPI.setClockDivider(SPI_CLOCK_DIV16);
        // BATTERYBALANCE_PRINTHEAD();
        // Serial.println("A");
        // Serial.println(millis()-checkLastMillis);
        checkLastMillis = millis();
        read_voltage();
        SPI.setClockDivider(SPI_CLOCK_DIV2);
    }
    else if (millis() - checkLastMillis >= checkInterval2)
    {
        Serial.println("A");
        // Serial.println(millis()-checkLastMillis);
        checkLastMillis = millis();
        // print_information_temp ();
    }

}
void BatteryBalance::start_charge_discharge_function(void)
{

    // polling_read();
    // if (first == 0)
    // {
    //     write_LTC3300();
    //     delay(50);
    //     excute_LTC3300();
    //     delay(50);
    //     first = 1;
    // }
    // else
    // {
    //     write_LTC3300();
    //     delay(50);
    // }
    // read_count++;
    // if (read_count == 5)
    // {
    //     read_count = 0;
    //     for (int i = 0; i < TOTAL_IC; i++)
    //     {
    //         for (int j = 0; j < CELL_NUM; j++)
    //         {
    //             if ((float)cell_codes[i][j] * 0.0001 > 4.1 || (float)cell_codes[i][j] * 0.0001 < 3.5)
    //             {
    //                 active_discharge_flag[i][j] = 0;
    //                 write_in_3300();
    //                 tx_comm_1[5] = 0X09;
    //                 crc();
    //                 tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim(CRC_1)) << 4);
    //                 write_LTC3300();
    //                 delay(50);
    //                 excute_LTC3300();
    //                 delay(50);
    //             }
    //             polling_read();
    //         }
    //     }
    // }
    // time_counter++;
    // start_balance_end_condition();
    // polling_read();
}
void BatteryBalance::take_a_break_funcion(void)
{
    polling_read();
    if (time_counter >= stop_balance || balance == First_detect_balance) // 30s
    {
        time_counter = 0;
        balance = Start_balance;
        end_condition();
        detect_charge_discharge();

        if ((high_vlotage - low_vlotage) >= 0.5)
        {
            start_balance = 323; // 120
            stop_balance = 2200; // 30
        }
        else if ((high_vlotage - low_vlotage) < 0.5 && (high_vlotage - low_vlotage) >= 0.4)
        {
            start_balance = 270; // 100
            stop_balance = 2200; // 30
        }
        else if ((high_vlotage - low_vlotage) < 0.4 && (high_vlotage - low_vlotage) >= 0.3)
        {
            start_balance = 215; // 80
            stop_balance = 2200; // 30
        }
        else if ((high_vlotage - low_vlotage) < 0.3 && (high_vlotage - low_vlotage) >= 0.2)
        {
            start_balance = 162; // 60
            stop_balance = 2200; // 30
        }
        else if ((high_vlotage - low_vlotage) < 0.2 && (high_vlotage - low_vlotage) >= 0.1)
        {
            start_balance = 108; // 40
            stop_balance = 2200; // 30
        }
        else if ((high_vlotage - low_vlotage) < 0.1 && (high_vlotage - low_vlotage) >= 0.05)
        {
            start_balance = 54;  // 20
            stop_balance = 2200; // 30
        }
        else
        {
            start_balance = 0;   // 15s
            stop_balance = 2200; // 30s
        }
        high_vlotage = 0.0;
        low_vlotage = 5.0;
    }
    polling_read();
}
