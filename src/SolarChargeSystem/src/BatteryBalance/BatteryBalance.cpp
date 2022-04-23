
#include "BatteryBalance.h"

BatteryBalance::BatteryBalance()
{
}

void BatteryBalance::begin(Stream *serial)
{
    // LTC6804_initialize(); // Initialize LTC6804 hardware
    this->serial = serial;
    //this->serialManager = serialManager;
    //serialManager->getSerialReceiver()->setOnReceiveCallback(serialOnReceive, this);
    applySPISpeed();

    setupLTC6804();    // Initialize 6804
    init_cfg();        // Initialize the 6804 configuration array to be written
    init_comm_1();     // Active Balance Config (Maybe?)
    wakeup_sleep();    // [LIB] PULL UP CS
    LTC6804_clrcell(); // Clear battery cell voltage.
    LTC6804_clraux();
    // delay(1000);
    read_voltage();
    delay(1000);
    read_voltage();
    restoreSPISpeed();
}

void BatteryBalance::run()
{
    polling_read();

    if (balance == Start_balance)
    {
        for (int i = 0; i < TOTAL_IC; i++)
            for (int j = 0; j < CELL_NUM; j++)
                Serial.print(active_discharge_flag[i][j]);
        Serial.println();
        /* Serial.println(time_counter);
          Serial.println(start_balance);*/
        Serial.println("Exe 30s");
        start_charge_discharge_function();
    }
    else if (balance == Take_break_balance)
    {
        Serial.println("Take break 30s");
        first = 0;
        delay(10);
        time_counter++;
        take_a_break_funcion();
        //***************************
    }
}

/**
 * @brief setup LTC6804. Alternative of LTC6804_initialize();
 * Because the offical setup function including Linduino board config.
 */
void BatteryBalance::setupLTC6804()
{
    // quikeval_SPI_connect();
    spi_enable(BMS_SPI_DIVIDE);
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
      active_discharge_flag [i][j] = 0;
  polling_read();
  for (int i = 0; i < 1; i++)
    for (int j = 0; j < 6; j++)
      if ((float)cell_codes[i][j] * 0.0001 > 2.5 && (float)cell_codes[i][j] * 0.0001 <= 4.3)
      {
        all_voltage = all_voltage + (float)cell_codes[i][j] * 0.0001;
        num_cell++;
      }
  avage_voltage = all_voltage / num_cell;
  polling_read();
  for (int i = 0; i < 1; i++)
  {
    for (int j = 0 ; j < 6; j++)
    {
      if ((float)cell_codes[i][j] * 0.0001 > avage_voltage && ((float)cell_codes[i][j] * 0.0001 <= 4.3))
        active_discharge_flag [i][j] = 2;
      else if ((float)cell_codes[i][j] * 0.0001 < avage_voltage && ((float)cell_codes[i][j] * 0.0001 >= 2.5))
        active_discharge_flag [i][j] = 3;
      else
        active_discharge_flag [i][j] = 0;
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
    num_high_cell = 0.0;
    all_high_voltage = 0.0;
    avage_high_voltage = 0.0;
    num_low_cell = 0.0;
    all_low_voltage = 0.0;
    avage_low_voltage = 0.0;
    for (int i = 0; i < 1; i++)
        for (int j = 0; j < 6; j++)
        {
            if (active_discharge_flag[i][j] == 2)
            {
                all_high_voltage = all_high_voltage + (float)cell_codes[i][j] * 0.0001;
                num_high_cell++;
            }
            else if (active_discharge_flag[i][j] == 3)
            {
                all_low_voltage = all_low_voltage + (float)cell_codes[i][j] * 0.0001;
                num_low_cell++;
            }
        }
    avage_high_voltage = all_high_voltage / num_high_cell;
    avage_low_voltage = all_low_voltage / num_low_cell;
    Serial.print("[BMS] HL: ");
    Serial.print(avage_high_voltage);
    Serial.print("/");
    Serial.print(avage_low_voltage);
    Serial.println();
    if ((avage_high_voltage - avage_low_voltage) <= 0.05) // 30s
    {
        interrupt_LTC3300();
        balance = 3;
        read_count = 0;
    }
}
/*********************************************************************************************/
void BatteryBalance::detect_high_low(void)
{
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if ((float)cell_codes[i][j] * 0.0001 >= high_vlotage && (float)cell_codes[i][j] * 0.0001 <= 4.3 && (float)cell_codes[i][j] * 0.0001 >= 2.5)
                high_vlotage = (float)cell_codes[i][j] * 0.0001;
            if ((float)cell_codes[i][j] * 0.0001 <= low_vlotage && (float)cell_codes[i][j] * 0.0001 >= 2.5 && (float)cell_codes[i][j] * 0.0001 <= 4.3)
                low_vlotage = (float)cell_codes[i][j] * 0.0001;
        }
    }
}
/******************************印出所有資訊***************************************************/
void BatteryBalance::print_information(void)
{
    Serial.print("[BMS] Volt");
    for (int i = 0; i < TOTAL_IC; i++)
        for (int j = 0; j < CELL_NUM; j++)
        {
            Serial.print(cell_codes[i][j] * 0.0001, 4);
            Serial.print(" ");
        }
    Serial.println();
}
void BatteryBalance::read_voltage(void)
{
    wakeup_idle();
    LTC6804_adcv();
    error = LTC6804_rdcv(0, TOTAL_IC, cell_codes);
    print_information();
}
void BatteryBalance::write_LTC3300(void)
{
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    wakeup_idle();
    tx_comm_1[0] = 0X8A;
    tx_comm_1[1] = 0x98;
    wrcomm_cofig();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
}
void BatteryBalance::excute_LTC3300(void)
{
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    wakeup_idle();
    tx_comm_1[0] = 0X8A;
    tx_comm_1[1] = 0xF8;
    wrcomm_cofig();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
}
void BatteryBalance::interrupt_LTC3300(void)
{
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    wakeup_idle();
    tx_comm_1[0] = 0X8A;
    tx_comm_1[1] = 0xE8;
    wrcomm_cofig();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
}
void BatteryBalance::polling_read(void)
{
    
    if (millis() - checkLastMillis >= checkInterval)
    {
        
        SPI.setClockDivider(SPI_CLOCK_DIV16);
        checkLastMillis = millis();
        read_voltage(); // 讀電池
        SPI.setClockDivider(SPI_CLOCK_DIV2);
    }
}

// 執行充放電
void BatteryBalance::start_charge_discharge_function(void)
{

    polling_read();
    if (first == 0)
    {
        write_LTC3300();
        delay(50);
        excute_LTC3300();
        delay(50);
        first = 1;
    }
    else
    {
        write_LTC3300();
        delay(50);
    }
    read_count++;
    if (read_count == 5)
    {
        read_count = 0;
        for (int i = 0; i < TOTAL_IC; i++)
        {
            for (int j = 0; j < CELL_NUM; j++)
            {
                if ((float)cell_codes[i][j] * 0.0001 > 4.3 || (float)cell_codes[i][j] * 0.0001 < 2.5)
                {
                    active_discharge_flag[i][j] = 0;
                    write_in_3300();
                    tx_comm_1[5] = 0X09;
                    crc();
                    tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim(CRC_1)) << 4);
                    write_LTC3300();
                    delay(50);
                    excute_LTC3300();
                    delay(50);
                }
                polling_read();
            }
        }
    }
    start_balance_end_condition();
    polling_read();
}

void BatteryBalance::take_a_break_funcion(void)
{
    if (balance == Take_break_balance) // 30s
    {
        for (int i = 0; i < TOTAL_IC; i++)
            for (int j = 0; j < CELL_NUM; j++)
            {
                if ((float)cell_codes[i][j] * 0.0001 >= 3.8)
                {
                    balance = Start_balance;
                    detect_high_low();
                    detect_charge_discharge();
                }
            }
        high_vlotage = 0.0;
        low_vlotage = 5.0;
    }
    polling_read();
}

void BatteryBalance::serialOnReceive(void *arg, String &payload)
{
    if (payload == "stop")
    {
        Serial.println("[BMS] stop");
        //interrupt_LTC3300();
        //balance = 4;
        
    }
    else if (payload == "start")
    {
        Serial.println("[BMS] start");
        //balance = First_detect_balance;
    }

    // char val = Serial.read();
    //      switch (val)
    //      {
    //      case '1': //中斷3300
    //          interrupt_LTC3300();
    //          balance = 4;
    //          break;
    //      case '2': //開始
    //          balance = First_detect_balance;
    //          break;
    //      case '3': //讀電池電壓
    //          // read_voltage();
    //          break;
    //      }

}

void BatteryBalance::applySPISpeed()
{
    SPI.setClockDivider(BMS_SPI_DIVIDE);
}

void BatteryBalance::restoreSPISpeed()
{
    SPI.setClockDivider(GLOBAL_SPI_DIVIDE);
}
