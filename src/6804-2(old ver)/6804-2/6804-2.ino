/* There is a list of functions as given in the LTC6804.cpp and for using those functions these libraries are necessary */

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC68042.h"
#include <SPI.h>



/******************************************************************************************/

/* All the basic functions are included as above */

/******************************************************************************************/

/******* GLOBAL VARIABLES ***********/
static const uint8_t crc4_tab[] = {               //crc4_table
  0x0, 0x7, 0xe, 0x9, 0xb, 0xc, 0x5, 0x2,
  0x1, 0x6, 0xf, 0x8, 0xa, 0xd, 0x4, 0x3,
};
const int CELL_NUM = 12;                          //每一個IC上有幾顆電池
const int TOTAL_IC = 2;                           //IC的數量
uint8_t tx_cfg[TOTAL_IC][6];                      //暫存器
//***************voltage***********************
uint16_t cell_codes[TOTAL_IC][CELL_NUM]={{0*10000,0*10000,3.6*10000,4*10000,3.6*10000,4*10000,0*10000,3.6*10000,3.6*10000,4*10000,4*10000,4*10000},{0*10000,3.6*10000,3.7*10000,4*10000,4*10000,4*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000}};                //電池的電壓數量
uint16_t cell_codes_temp[TOTAL_IC][CELL_NUM]={{0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000},{0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000,0*10000}};
uint16_t GPIO_codes[TOTAL_IC][6];                 //GPIO的電壓訊息
//***********************************************
int discharge_flag [TOTAL_IC][12] = {0};          //被動放電旗標
uint8_t  re_cfg[TOTAL_IC][8];                     //忘記了
uint8_t  active_cfg_1[TOTAL_IC][8],active_cfg_2[TOTAL_IC][8]; //主動暫存器-1//主動暫存器-2
float all_voltage[5] = {0.0,0.0,0.0,0.0,0.0};                           //總電壓
float avage_voltage[5]={0.0,0.0,0.0,0.0,0.0}; //平均電壓
uint8_t active_discharge_flag [TOTAL_IC][12] = {0}; //主動充放電旗標
uint8_t error;                                      //錯誤碼
int balance=-2;
int time_counter=0,read_count=0;
int first=0,check_step=0;
int timer0_count=0,step_2_count=0;
float num_cell[5]={0.0,0.0,0.0,0.0,0.0};
float high_vlotage[6]={0.0,0.0,0.0,0.0,0.0,0.0},low_vlotage[6]={5.0,5.0,5.0,5.0,5.0,5.0};

int charge_discharge_flag=1,balance_flag[3]={0,0,0},balance2_flag[2]={0,0};
// 開始平衡的初始時間
int start_balance[6]={120,120,120,120,120,120},stop_balance=2200;
//******************************************************************************************
uint8_t tx_comm_1[6],tx_comm_2[6],tx_comm_3[6],tx_comm_4[6];//主動暫存器-1~6
uint16_t CRC_1 = 0, CRC_2 = 0, CRC_3 = 0, CRC_4 = 0; //CRC碼

unsigned long checkLastMillis = 0;
unsigned int checkInterval = 1000;
unsigned int checkInterval2 = 1100;

//*************************初始暫存器***************************************************

void init_cfg() {
  for (int i = 0; i < TOTAL_IC; i++) {
    tx_cfg[i][0] = 0XFC;
    tx_cfg[i][1] = 0x00;
    tx_cfg[i][2] = 0x00;
    tx_cfg[i][3] = 0x00;
    tx_cfg[i][4] = 0x00;
    tx_cfg[i][5] = 0x10;
  }
}
//**************************被動平衡的東西*******************************
void passive_discharge_dirction()
{
  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
  {
    for (int i = 0; i < CELL_NUM; i++)
    {
      if (cell_codes[current_ic][i] * 0.0001 > 3)
        discharge_flag [current_ic][i] = 1 ;
      else
        discharge_flag [current_ic][i] = 0 ;
    }
  }
}

void passive_discharge ( )
{
  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
  {
    for (int i = 0; i < CELL_NUM; i++)
    {
      if (discharge_flag[current_ic][i] == 1)
      {
        if (i < 8)
          tx_cfg[current_ic][4] = (tx_cfg[current_ic][4] | (1 << i));
        else
          tx_cfg[current_ic][5] = (tx_cfg[current_ic][5] | (1 << (i - 8)));
      }
      if (discharge_flag[current_ic][i] == 0)
      {
        if (i < 8)
          tx_cfg[current_ic][4] = tx_cfg[current_ic][4] & (0xFF ^ (1 << i));
        else
          tx_cfg[current_ic][5] = tx_cfg[current_ic][5] & (0xFF ^ (1 << (i - 8)));
      }
      //Serial.println( tx_cfg[current_ic][4]);
    }
  }
}
//*****************************印出電池地壓****************************************************
void print_cells()
{
  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
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
void print_GPIO()
{
  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
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

void init_comm_1() {   // LTC3300第一顆
  tx_comm_1[0] = 0X8A;
  tx_comm_1[1] = 0x98;
  tx_comm_1[2] = 0x80;
  tx_comm_1[3] = 0xB8;
  tx_comm_1[4] = 0x8B;
  tx_comm_1[5] = 0xF8;
}
void init_comm_2() {   // LTC3300第二顆
  tx_comm_2[0] = 0X8B;
  tx_comm_2[1] = 0x08;
  tx_comm_2[2] = 0x80;
  tx_comm_2[3] = 0xF9;
  tx_comm_2[4] = 0xF0;
  tx_comm_2[5] = 0x59;
}
void init_comm_3() {   // LTC3300第三顆
  tx_comm_3[0] = 0X8A;
  tx_comm_3[1] = 0x98;
  tx_comm_3[2] = 0x8B;
  tx_comm_3[3] = 0xB8;
  tx_comm_3[4] = 0x80;
  tx_comm_3[5] = 0xF9;
}
void init_comm_4() {   // LTC3300第四顆
  tx_comm_4[0] = 0X80;
  tx_comm_4[1] = 0x08;
  tx_comm_4[2] = 0x8B;
  tx_comm_4[3] = 0xF9;
  tx_comm_4[4] = 0xF0;
  tx_comm_4[5] = 0x59;
}

//********************寫入主動暫存器****************************
void wrcomm_cofig (void)
{
  for (int i = 0; i < TOTAL_IC; i++) {
    if (i == 0)
    {
      LTC6804_wrcomm(i, tx_comm_1);
      LTC6804_stcomm(i , 9);
      // error =LTC6804_rdcomm(TOTAL_IC ,active_cfg_1);
      LTC6804_wrcomm(i, tx_comm_2);
      LTC6804_stcomm(i , 6);
    }
    else
    {
      LTC6804_wrcomm(i, tx_comm_3);
      LTC6804_stcomm(i , 9);
      LTC6804_wrcomm(i, tx_comm_4);
      LTC6804_stcomm(i , 6);
    }
  }
  //error =LTC6804_rdcomm(TOTAL_IC ,active_cfg_2);*/
}
//*************************主動所有功能****************************

//**************************crc************************************
void crc (void)
{
  CRC_1 = ((tx_comm_1[2] & 0X0F) << 8) + ((tx_comm_1[3] & 0XF0)) + (tx_comm_1[4] & 0X0F);
  CRC_2 = ((tx_comm_2[0] & 0X0F) << 8) + ((tx_comm_2[1] & 0XF0)) + (tx_comm_2[2] & 0X0F);
  CRC_3 = ((tx_comm_3[2] & 0X0F) << 8) + ((tx_comm_3[3] & 0XF0)) + (tx_comm_3[4] & 0X0F);
  CRC_4 = ((tx_comm_4[0] & 0X0F) << 8) + ((tx_comm_4[1] & 0XF0)) + (tx_comm_4[2] & 0X0F);
}
//*************************判斷主動充放電*********************************
void HIGH_VOLTAGE_VS_LOW_VOLTAGE (void)
{
    for (int i = 0; i < 6; i++)
      num_cell[i]=0.0;
    for (int i = 0; i < 6; i++)
        all_voltage[i]=0.0;
    for (int i = 0; i < 6; i++)
        avage_voltage[i]=0.0;
  for (int i = 0; i < TOTAL_IC; i++)
    for (int j = 0; j < CELL_NUM; j++)
        active_discharge_flag [i][j]=0;
        polling_read();
    if(charge_discharge_flag==1)
    {
      if(balance_flag[0]==0)
      {
        for (int i = 0; i < 1; i++)
        {
          for (int j = 0; j < 6; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[0] = all_voltage[0] + (float)cell_codes[i][j] * 0.0001;
              num_cell[0]++;
            }
          }
        }
      }
      polling_read();
      if(balance_flag[1]==0)
      {
        for (int i = 0; i < 1; i++)
        {
          for (int j = 6; j < 12; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[1] = all_voltage[1] + (float)cell_codes[i][j] * 0.0001;
              num_cell[1]++;
            }
          }
        }
      }
      polling_read();
      if(balance_flag[2]==0)
      {
        for (int i = 1; i < 2; i++)
        {
          for (int j = 0; j < 12; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[2] = all_voltage[2] + (float)cell_codes[i][j] * 0.0001;
              num_cell[2]++;
            }
          }
        }
      }
      polling_read();
    }
    else if(charge_discharge_flag==2)
    {
      if(balance2_flag[0]==0)
      {
        for (int i = 0; i < 1; i++)
        {
          for (int j = 0; j < 6; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[0] = all_voltage[0] + (float)cell_codes[i][j] * 0.0001;
              num_cell[0]++;
            }
          }
        }
        polling_read();
        for (int i = 0; i < 1; i++)
        {
          for (int j = 0; j < 12; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[3] = all_voltage[3] + (float)cell_codes[i][j] * 0.0001;
              num_cell[3]++;
            }
          }
        }
        polling_read();
      }
      if(balance2_flag[1]==0)
      {
        for (int i = 0; i < 1; i++)
        {
          for (int j = 6; j < 12; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[1] = all_voltage[1] + (float)cell_codes[i][j] * 0.0001;
              num_cell[1]++;
            }
          }
        }
        polling_read();
        for (int i = 0; i < 1; i++)
        {
          for (int j = 6; j < 12; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[4] = all_voltage[4] + (float)cell_codes[i][j] * 0.0001;
              num_cell[4]++;
            }
          }
        }
        polling_read();
        for (int i = 1; i < 2; i++)
        {
          for (int j = 0; j < 6; j++)
          {
            if((float)cell_codes[i][j] * 0.0001>3.5&&(float)cell_codes[i][j] * 0.0001<4.1)
            {
              all_voltage[4] = all_voltage[4] + (float)cell_codes[i][j] * 0.0001;
              num_cell[4]++;
            }
          }
        }
        polling_read();
      }
    }
    for (int i = 0; i < 5; i++)
        avage_voltage[i] = all_voltage[i] / num_cell[i];
    if(charge_discharge_flag==1)
    {
      if(balance_flag[0]==0)
      {
        for (int i = 0; i < 1; i++)
        {
          for (int j = 0 ; j < 6; j++)
          {
            if ((float)cell_codes[i][j] * 0.0001 > avage_voltage[0]&&((float)cell_codes[i][j] * 0.0001 <= 4.1))
              active_discharge_flag [i][j] = 2;
            else if ((float)cell_codes[i][j] * 0.0001 < avage_voltage[0]&&((float)cell_codes[i][j] * 0.0001 >= 3.5))
              active_discharge_flag [i][j] = 3;
            else
              active_discharge_flag [i][j] = 0; 
          }
        }
      }
      polling_read();
      if(balance_flag[1]==0)
      {
        for (int i = 0; i < 1; i++)
        {
          for (int j = 6 ; j < 12; j++)
          {
            if ((float)cell_codes[i][j] * 0.0001 > avage_voltage[1]&&((float)cell_codes[i][j] * 0.0001 <= 4.1))
              active_discharge_flag [i][j] = 2;
            else if ((float)cell_codes[i][j] * 0.0001 < avage_voltage[1]&&((float)cell_codes[i][j] * 0.0001 >= 3.5))
              active_discharge_flag [i][j] = 3;
            else
              active_discharge_flag [i][j] = 0; 
          }
        }
      }
      polling_read();
      if(balance_flag[2]==0)
      {
        for (int i = 1; i < 2; i++)
        {
          for (int j = 0 ; j < 12; j++)
          {
            if ((float)cell_codes[i][j] * 0.0001 > avage_voltage[2]&&((float)cell_codes[i][j] * 0.0001 <= 4.1))
              active_discharge_flag [i][j] = 2;
            else if ((float)cell_codes[i][j] * 0.0001 < avage_voltage[2]&&((float)cell_codes[i][j] * 0.0001 >= 3.5))
              active_discharge_flag [i][j] = 3;
            else
              active_discharge_flag [i][j] = 0; 
          }
        }
      }
      polling_read();
    }
    else if(charge_discharge_flag==2)
    {
      if(balance2_flag[0]==0)
      {
        if (avage_voltage[0] > avage_voltage[3]&&(avage_voltage[0] <= 4.1))
        {
          for (int i = 0; i < 1; i++)
            for (int j = 2; j < 6; j++)
              active_discharge_flag [i][j]=2;
        }
        else if (avage_voltage[0] < avage_voltage[3]&&(avage_voltage[0] >= 3.5))
        {
          for (int i = 0; i < 1; i++)
            for (int j = 2; j < 6; j++)
              active_discharge_flag [i][j]=3;
        }
        else
        {
          for (int i = 0; i < 1; i++)
            for (int j = 2; j < 6; j++)
              active_discharge_flag [i][j]=0;
        }
      }
      polling_read();
      if(balance2_flag[1]==0)
      {
        if (avage_voltage[1] > avage_voltage[4]&&(avage_voltage[1] <= 4.1))
        {
          for (int i = 0; i < 1; i++)
            for (int j = 7; j < 12; j++)
              active_discharge_flag [i][j]=2;
        }
        else if (avage_voltage[1] < avage_voltage[4]&&(avage_voltage[1] >= 3.5))
        {
          for (int i = 0; i < 1; i++)
            for (int j = 7; j < 12; j++)
              active_discharge_flag [i][j]=3;
        }
        else
        {
          for (int i = 0; i < 1; i++)
            for (int j = 7; j < 12; j++)
              active_discharge_flag [i][j]=0;
        }
      }
      polling_read();
    }
  /*for (int i = 0; i < TOTAL_IC; i++)
    for (int j = 0; j < CELL_NUM; j++)
        Serial.print(active_discharge_flag [i][j]);
  Serial.println( );*/
}
void write_in_3300(void)
{
    for (int j = 0; j < CELL_NUM; j++)
    {
      if (j == 0 || j == 1)
      {
        if (j == 0)
          tx_comm_1[2] = 0x80;
        tx_comm_1[2] = tx_comm_1[2] | active_discharge_flag [0][j] << (2 - 2 * j);
      }
      else if (j == 2 || j == 3)
      {
        if (j == 2)
          tx_comm_1[3] = 0x08;
        tx_comm_1[3] = tx_comm_1[3] | active_discharge_flag [0][j] << (10 - 2 * j);
      }
      else if (j == 4 || j == 5)
      {
        if (j == 4)
          tx_comm_1[4] = 0x80;
        tx_comm_1[4] = tx_comm_1[4] | active_discharge_flag [0][j] << (10 - 2 * j);
      }
      else if (j == 6 || j == 7)
      {
        if (j == 6)
          tx_comm_2[0] = 0x80;
        tx_comm_2[0] = tx_comm_2[0] | active_discharge_flag [0][j] << (14 - 2 * j);
      }
      else if (j == 8 || j == 9)
      {
        if (j == 8)
          tx_comm_2[1] = 0x08;
        tx_comm_2[1] = tx_comm_2[1] | active_discharge_flag [0][j] << (22 - 2 * j);
      }
      else if (j == 10 || j == 11)
      {
        if (j == 10)
          tx_comm_2[2] = 0x80;
        tx_comm_2[2] = tx_comm_2[2] | active_discharge_flag [0][j] << (22 - 2 * j);
      }
    }
    for (int j = 0; j < CELL_NUM; j++)
    {
      if (j == 0 || j == 1)
      {
        if (j == 0)
          tx_comm_3[2] = 0x80;
        tx_comm_3[2] = tx_comm_3[2] | active_discharge_flag [1][j] << (2 - 2 * j);
      }
      else if (j == 2 || j == 3)
      {
        if (j == 2)
          tx_comm_3[3] = 0x08;
        tx_comm_3[3] = tx_comm_3[3] | active_discharge_flag [1][j] << (10 - 2 * j);
      }
      else if (j == 4 || j == 5)
      {
        if (j == 4)
          tx_comm_3[4] = 0x80;
        tx_comm_3[4] = tx_comm_3[4] | active_discharge_flag [1][j] << (10 - 2 * j);
      }
    }
}
uint8_t crc_new_logisim (uint16_t x)
{
  int crc[12];
  int crc_second[4];
  for (int i = 0 ; i < 12 ; i++ )
  {
    crc[i] = (x & (0x01 << i)) >> i;
  }
  crc_second[3] = (!(((((crc[6] ^ crc[10])^crc[9])^crc[2]) ^ ((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8]))) ^ (((((crc[7] ^ crc[11])^crc[10])^crc[3]) ^ ((crc[6] ^ crc[10])^crc[9])) ^ 0)));
  crc_second[2] = (!((((((crc[6] ^ crc[10])^crc[9])^crc[2]) ^ ((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8]))) ^ 0) ^ ((((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8]))^crc[1]) ^ (((crc[11] ^ crc[8])^crc[4]) ^ ((crc[7] ^ crc[11])^crc[10])))));
  crc_second[1] = (!((((((crc[11] ^ crc[8])^crc[4]) ^ ((crc[7] ^ crc[11])^crc[10]))^crc[0]) ^ ((((crc[7] ^ crc[11])^crc[10])^crc[3]) ^ ((crc[6] ^ crc[10])^crc[9]))) ^ (((((crc[9] ^ crc[5]) ^ (crc[11] ^ crc[8]))^crc[1]) ^ (((crc[11] ^ crc[8])^crc[4]) ^ ((crc[7] ^ crc[11])^crc[10]))) ^ 0)));
  crc_second[0] = (!((((((crc[11] ^ crc[8])^crc[4]) ^ ((crc[7] ^ crc[11])^crc[10]))^crc[0]) ^ ((((crc[7] ^ crc[11])^crc[10])^crc[3]) ^ ((crc[6] ^ crc[10])^crc[9]))) ^ 0));
  return (crc_second[3] << 3 | crc_second[2] << 2 | crc_second[1] << 1 | crc_second[0]);
}
void change_information(void)
{
  uint8_t temp[8]={0};
  for( int i = 2 ; i < 6 ; i++)
    temp[i-2]=tx_comm_1[i];
  for( int i = 0 ; i < 4 ; i++)
    temp[i+4]=tx_comm_2[i];
   for( int i = 0 ; i < 4 ; i++)
   {
    tx_comm_1[i+2]=temp[i+4];
    tx_comm_2[i]=temp[i+0];
   }
}
void detect_charde_discharge(void)
{
   HIGH_VOLTAGE_VS_LOW_VOLTAGE();
   polling_read();
   write_in_3300();
  tx_comm_1[5] = 0X09;
  tx_comm_2[3] = 0X08;
  tx_comm_3[5] = 0X09;
  //tx_comm_4[3] = 0X09;
  change_information();
  polling_read();
  crc();
  tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim (CRC_1)) << 4) ;
  tx_comm_2[3] = tx_comm_2[3] | (crc_new_logisim (CRC_2)) << 4;
  tx_comm_3[5] = (tx_comm_3[5] | (crc_new_logisim (CRC_3)) << 4) ;
  //tx_comm_4[3] = tx_comm_4[3] | (crc_new_logisim (CRC_4)) << 4;
}
//*********************************************************************************************
void start_balance_end_condition(void)
{
            if(charge_discharge_flag==1)
            {
              if((time_counter>=start_balance[0])||(time_counter>=start_balance[1])||(time_counter>=start_balance[2]))
              {
                    if((time_counter>=start_balance[0]))
                    {
                        for (int i = 0; i < 1; i++)
                            for (int j = 0; j < 6; j++)
                                active_discharge_flag [i][j]=0;
                    }
                    if((time_counter>=start_balance[1]))
                    {
                        for (int i = 0; i < 1; i++)
                            for (int j = 6; j < 12; j++)
                                active_discharge_flag [i][j]=0;
                    }
                    if((time_counter>=start_balance[2]))
                    {
                        for (int i = 1; i < 2; i++)
                            for (int j = 0; j < 6; j++)
                                active_discharge_flag [i][j]=0;
                    }
                     write_in_3300();
                     tx_comm_1[5] = 0X09;
                     tx_comm_2[3] = 0X08;
                     tx_comm_3[5] = 0X09;
                    //tx_comm_4[3] = 0X09;
                     change_information();
                     crc();
                     tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim (CRC_1)) << 4) ;
                     tx_comm_2[3] = tx_comm_2[3] | (crc_new_logisim (CRC_2)) << 4;
                     tx_comm_3[5] = (tx_comm_3[5] | (crc_new_logisim (CRC_3)) << 4) ;
                     //tx_comm_4[3] = tx_comm_4[3] | (crc_new_logisim (CRC_4)) << 4;
                     write_LTC3300();
                      delay(50);
                     excute_LTC3300();
                      delay(50);
              }
              
              if((time_counter>=start_balance[0])&&(time_counter>=start_balance[1])&&(time_counter>=start_balance[2]) )//30s
              {
                interrupt_LTC3300();
                balance=1;
                read_count=0;
                time_counter=0;
              }
            }
            else if(charge_discharge_flag==2)
            {
              if((time_counter>=start_balance[3])||(time_counter>=start_balance[4]))
              {
                  if((time_counter>=start_balance[3]))
                  {
                      for (int i = 0; i < 1; i++)
                         for (int j = 0; j < 6; j++)
                             active_discharge_flag [i][j]=0;
                  }
                  if((time_counter>=start_balance[4]))
                  {
                      for (int i = 0; i < 1; i++)
                          for (int j = 6; j < 12; j++)
                              active_discharge_flag [i][j]=0;
                  }
                  write_in_3300();
                  tx_comm_1[5] = 0X09;
                  tx_comm_2[3] = 0X08;
                  tx_comm_3[5] = 0X09;
                  //tx_comm_4[3] = 0X09;
                  change_information();
                  crc();
                  tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim (CRC_1)) << 4) ;
                  tx_comm_2[3] = tx_comm_2[3] | (crc_new_logisim (CRC_2)) << 4;
                  tx_comm_3[5] = (tx_comm_3[5] | (crc_new_logisim (CRC_3)) << 4) ;
                  //tx_comm_4[3] = tx_comm_4[3] | (crc_new_logisim (CRC_4)) << 4;
                  write_LTC3300();
                  delay(50);
                  excute_LTC3300();
                  delay(50);
              }
              if((time_counter>=start_balance[3])&&(time_counter>=start_balance[4]))//30s
              {
                interrupt_LTC3300();
                balance=1;
                read_count=0;
                time_counter=0;
              }
           }
           else if(charge_discharge_flag==3)
            {
              if(time_counter>=start_balance[5] )//30s
              {
                interrupt_LTC3300();
                balance=1;
                read_count=0;
                time_counter=0;
              }
           }
           else if(charge_discharge_flag==4)
           {
              interrupt_LTC3300();
              balance=1;
              read_count=0;
              time_counter=0;
           }
}
/*********************************************************************************************/
void end_condition(void)
{
  if(charge_discharge_flag==1)
  {
    if(balance_flag[0]==0)
    {
      for (int i = 0; i < 1; i++)
      {
        for (int j = 0; j < 6; j++)
        {
          if((float)cell_codes[i][j] * 0.0001>=high_vlotage[0]&&(float)cell_codes[i][j] * 0.0001<=4.1&&(float)cell_codes[i][j] * 0.0001>=3.5)
            high_vlotage[0]=(float)cell_codes[i][j] * 0.0001;
          if((float)cell_codes[i][j] * 0.0001<=low_vlotage[0]&&(float)cell_codes[i][j] * 0.0001>=3.5&&(float)cell_codes[i][j] * 0.0001<=4.1)
            low_vlotage[0]=(float)cell_codes[i][j] * 0.0001;
        }
      }
    }
    polling_read();
    if(balance_flag[1]==0)
    {
      for (int i = 0; i < 1; i++)
      {
        for (int j = 6; j < 12; j++)
        {
          if((float)cell_codes[i][j] * 0.0001>=high_vlotage[1]&&(float)cell_codes[i][j] * 0.0001<=4.1&&(float)cell_codes[i][j] * 0.0001>=3.5)
            high_vlotage[1]=(float)cell_codes[i][j] * 0.0001;
          if((float)cell_codes[i][j] * 0.0001<=low_vlotage[1]&&(float)cell_codes[i][j] * 0.0001>=3.5&&(float)cell_codes[i][j] * 0.0001<=4.1)
            low_vlotage[1]=(float)cell_codes[i][j] * 0.0001;
        }
      }
    }
    polling_read();
    if(balance_flag[2]==0)
    {
      for (int i = 1; i < 2; i++)
      {
        for (int j = 0; j < 6; j++)
        {
          if((float)cell_codes[i][j] * 0.0001>=high_vlotage[2]&&(float)cell_codes[i][j] * 0.0001<=4.1&&(float)cell_codes[i][j] * 0.0001>=3.5)
            high_vlotage[2]=(float)cell_codes[i][j] * 0.0001;
          if((float)cell_codes[i][j] * 0.0001<=low_vlotage[2]&&(float)cell_codes[i][j] * 0.0001>=3.5&&(float)cell_codes[i][j] * 0.0001<=4.1)
            low_vlotage[2]=(float)cell_codes[i][j] * 0.0001;
        }
      }
    }
    polling_read();
  }
  else if(charge_discharge_flag==2)
  {
    if(balance2_flag[0]==0)
    {
      for (int i = 0; i < 1; i++)
      {
        for (int j = 0; j < 12; j++)
        {
          if((float)cell_codes[i][j] * 0.0001>=high_vlotage[3]&&(float)cell_codes[i][j] * 0.0001<=4.1&&(float)cell_codes[i][j] * 0.0001>=3.5)
            high_vlotage[3]=(float)cell_codes[i][j] * 0.0001;
          if((float)cell_codes[i][j] * 0.0001<=low_vlotage[3]&&(float)cell_codes[i][j] * 0.0001>=3.5&&(float)cell_codes[i][j] * 0.0001<=4.1)
            low_vlotage[3]=(float)cell_codes[i][j] * 0.0001;
        }
        polling_read();
      }
    }
    if(balance2_flag[1]==0)
    {
      for (int i = 0; i < 1; i++)
      {
        for (int j = 6; j < 12; j++)
        {
          if((float)cell_codes[i][j] * 0.0001>=high_vlotage[4]&&(float)cell_codes[i][j] * 0.0001<=4.1&&(float)cell_codes[i][j] * 0.0001>=3.5)
            high_vlotage[4]=(float)cell_codes[i][j] * 0.0001;
          if((float)cell_codes[i][j] * 0.0001<=low_vlotage[4]&&(float)cell_codes[i][j] * 0.0001>=3.5&&(float)cell_codes[i][j] * 0.0001<=4.1)
            low_vlotage[4]=(float)cell_codes[i][j] * 0.0001;
        }
      }
      polling_read();
      for (int i = 1; i < 2; i++)
      {
        for (int j = 0; j < 6; j++)
        {
          if((float)cell_codes[i][j] * 0.0001>=high_vlotage[4]&&(float)cell_codes[i][j] * 0.0001<=4.1&&(float)cell_codes[i][j] * 0.0001>=3.5)
            high_vlotage[4]=(float)cell_codes[i][j] * 0.0001;
          if((float)cell_codes[i][j] * 0.0001<=low_vlotage[4]&&(float)cell_codes[i][j] * 0.0001>=3.5&&(float)cell_codes[i][j] * 0.0001<=4.1)
            low_vlotage[4]=(float)cell_codes[i][j] * 0.0001;
        }
      }
      polling_read();
    }
  }
  else if(charge_discharge_flag==3)
  {
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 12; j++)
      {
        if((float)cell_codes[i][j] * 0.0001>=high_vlotage[5]&&(float)cell_codes[i][j] * 0.0001<=4.1&&(float)cell_codes[i][j] * 0.0001>=3.5)
          high_vlotage[5]=(float)cell_codes[i][j] * 0.0001;
        if((float)cell_codes[i][j] * 0.0001<=low_vlotage[5]&&(float)cell_codes[i][j] * 0.0001>=3.5&&(float)cell_codes[i][j] * 0.0001<=4.1)
          low_vlotage[5]=(float)cell_codes[i][j] * 0.0001;
      }
    polling_read();
    }
  }
}
/******************************印出所有資訊***************************************************/
void print_information (void)
{
  for (int i = 0; i < TOTAL_IC; i++)
  {
     for (int j = 0; j < CELL_NUM; j++)
     {
        Serial.print(cell_codes[i][j] * 0.0001,4);
        //cell_codes_temp[i][j]=cell_codes[i][j];
     }
  }
  //Serial.println( );
  /*if(balance==0)
  Serial.print("excute");
  else if(balance==1)
  Serial.print("pause ");
  else if(balance==4)
  Serial.print("finish");*/
}
void read_voltage(void)
{
   wakeup_idle();
   LTC6804_adcv();
   //error = LTC6804_rdcv(0, TOTAL_IC, cell_codes);
   print_information ();
}
void write_LTC3300(void)
{
     wakeup_idle();
     tx_comm_1[0] = 0X8A;
     tx_comm_1[1] = 0x98;
     tx_comm_3[0] = 0X8A;
     tx_comm_3[1] = 0x98;
     wrcomm_cofig ();
}
void excute_LTC3300(void)
{
     wakeup_idle();
     tx_comm_1[0] = 0X8A;
     tx_comm_1[1] = 0xF8;
     tx_comm_3[0] = 0X8A;
     tx_comm_3[1] = 0xF8;
     wrcomm_cofig ();
}
void interrupt_LTC3300(void)
{
     wakeup_idle();
     tx_comm_1[0] = 0X8A;
     tx_comm_1[1] = 0xE8;
    // detect_charde_discharge();
     wrcomm_cofig ();
}
void polling_read (void)
{
  if (millis()-checkLastMillis >= checkInterval&&millis()-checkLastMillis <= checkInterval2)
      {
        //Serial.println(millis()-checkLastMillis);
        checkLastMillis = millis();
        read_voltage();
      }
     else if(millis()-checkLastMillis >= checkInterval2)
     {
      //Serial.println(millis()-checkLastMillis);
      checkLastMillis = millis();
      //print_information_temp ();
     }
}
void step_function1(void)
{
  if(first==0)
            {
              polling_read();
              write_LTC3300();
              delay(50);
              polling_read();
              excute_LTC3300();
              delay(50);
              first=1;
              polling_read();
            }
            else
            {
              write_LTC3300();
              delay(50);
              polling_read();
            }
              read_count++;
            if(read_count==5)
            {
              read_count=0;
              for (int i = 0; i < TOTAL_IC; i++)
              {
                for (int j = 0; j < CELL_NUM; j++)
                {
                  if((float)cell_codes[i][j] * 0.0001>4.1||(float)cell_codes[i][j] * 0.0001<3.5)
                  {
                    polling_read();
                     active_discharge_flag [i][j]=0;
                     write_in_3300();
                     tx_comm_1[5] = 0X09;
                     tx_comm_2[3] = 0X08;
                     tx_comm_3[5] = 0X09;
                    //tx_comm_4[3] = 0X09;
                    polling_read();
                     change_information();
                     polling_read();
                     crc();
                     tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim (CRC_1)) << 4) ;
                     tx_comm_2[3] = tx_comm_2[3] | (crc_new_logisim (CRC_2)) << 4;
                     tx_comm_3[5] = (tx_comm_3[5] | (crc_new_logisim (CRC_3)) << 4) ;
                     //tx_comm_4[3] = tx_comm_4[3] | (crc_new_logisim (CRC_4)) << 4;
                     write_LTC3300();
                     polling_read();
                      delay(50);
                      polling_read();
                     excute_LTC3300();
                      delay(50);
                      polling_read();
                  }
                  polling_read();
                }
              }
            }
            time_counter++;
            polling_read();
            start_balance_end_condition();
            polling_read();
}
void step_funcion2(void)
{
            if(time_counter>=stop_balance||balance==-1)//30s
            {
              time_counter=0;
              balance=0;
              end_condition();
              polling_read();
              /*for (int i = 0; i < 4; i++)
                {
                  Serial.println(high_vlotage[i],4);
                  Serial.println(low_vlotage[i],4);
                }*/
              if((charge_discharge_flag==3)&&((high_vlotage[5]-low_vlotage[5])>=0.1))
              {
                for(int i=0;i<3;i++)
                  balance_flag[i]=0;
                for(int i=0;i<2;i++)
                  balance2_flag[i]=0;
                charge_discharge_flag=1;
                polling_read();
              }
              polling_read();
              if(charge_discharge_flag==1)
              {
                for(int i=0;i<3;i++)
                  balance_flag[i]=0;
                for (int i = 0; i < 3; i++)
                {
                  //Serial.println((high_vlotage[i]-low_vlotage[i]),4);
                  if((high_vlotage[i]-low_vlotage[i])<0.1)
                    balance_flag[i]=1;
                    polling_read();
                }
                /*for (int i = 0; i < 3; i++)
                    Serial.println(balance_flag[i]);*/
                if((balance_flag[0])&&(balance_flag[1])&&(balance_flag[2]))
                {
                  interrupt_LTC3300();
                  charge_discharge_flag++;
                  polling_read();
                }
                polling_read();
              }
              else if(charge_discharge_flag==2)
              {
                for(int i=0;i<2;i++)
                  balance2_flag[i]=0;
                step_2_count++;
                //Serial.println(step_2_count);
                polling_read();
                if(step_2_count>=5)
                {
                  polling_read();
                  step_2_count=0;
                  charge_discharge_flag=1;
                  for(int i=0;i<3;i++)
                  balance_flag[i]=0;
                  polling_read();
                   end_condition();
                }
                polling_read();
                for (int i = 3; i < 5; i++)
                {
                  //Serial.println((high_vlotage[i]-low_vlotage[i]),4);
                  if((high_vlotage[i]-low_vlotage[i])<0.1)
                    balance2_flag[i-3]=1;
                    polling_read();
                }
                 if((balance2_flag[0])&&(balance2_flag[1]))
                {
                  interrupt_LTC3300();
                  charge_discharge_flag=3;
                  polling_read();
                }
              }
              else if(charge_discharge_flag==3)
              {
                if((high_vlotage[5]-low_vlotage[5])<=0.1)
                {
                  interrupt_LTC3300();
                  charge_discharge_flag++;
                  polling_read();
                }
              }
              else if(charge_discharge_flag==4)
              {
                 charge_discharge_flag=1;
                 balance=4;
                 polling_read();
              }
              polling_read();
              detect_charde_discharge();
              polling_read();
              for (int i = 0; i < 6; i++)
              {
                if((high_vlotage[i]-low_vlotage[i])>=0.5)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=323;//120
                  else
                    start_balance[i]=648;//120
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.5&&(high_vlotage[i]-low_vlotage[i])>=0.4)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=270;//100
                  else
                    start_balance[i]=540;//100
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.4&&(high_vlotage[i]-low_vlotage[i])>=0.3)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=215;//80
                  else
                    start_balance[i]=432;//80
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.3&&(high_vlotage[i]-low_vlotage[i])>=0.2)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=162;//60
                  else
                    start_balance[i]=324;//60
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.2&&(high_vlotage[i]-low_vlotage[i])>=0.1)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=108;//40
                  else
                    start_balance[i]=216;//40
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.1&&(high_vlotage[i]-low_vlotage[i])>=0.05)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=54;//20 
                  else
                    start_balance[i]=108;//20 
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.05&&(high_vlotage[i]-low_vlotage[i])>=0.01)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=27;//10s
                  else
                    start_balance[i]=54;//10s
                  stop_balance=2200;//30s
                  polling_read();
                }
                else
                {
                  start_balance[i]=0;//15s
                  stop_balance=2200;//30s
                  polling_read();
                }
                polling_read();
              }
              polling_read();
              for (int i = 0; i < 6; i++)
              {
                high_vlotage[i]=0.0;
                low_vlotage[i]=5.0;
              }
              polling_read();
              /*for (int i = 0; i < 6; i++)
              {
                Serial.println(start_balance[i]);
              }*/
              //Serial.println(charge_discharge_flag);
            }
}
void setup()
{
  Serial.begin(9600);
  /*cli();//stop interrupts

  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);


  sei();//allow interrupts*/
  LTC6804_initialize();  //Initialize LTC6804 hardware
  init_cfg();            //Initialize the 6804 configuration array to be written
  init_comm_1();
  init_comm_2();
  init_comm_3();
  init_comm_4();
  wakeup_sleep();
  LTC6804_clrcell();
  LTC6804_clraux();
  //delay(1000);
  //read_voltage();
  //delay(1000);
  //read_voltage(); 
  //delay(1000);
  //read_voltage(); 
   //delay(1000);
}
/*ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
  timer0_count++;
  if(timer0_count==2000)
  {
    timer0_count=0;
    //Serial.println(" ");
    read_voltage();
  }
}*/

void loop()
{       
  
        polling_read();
        //Serial.println(charge_discharge_flag);
        if (Serial.available())
        {
          char val = Serial.read();
          switch (val)
          {
            case '1':  //中斷3300
            interrupt_LTC3300();
            balance=4;
            //Serial.println("STOP");
            break; 
            case '2':  //開始
            balance=-1;
            //Serial.println("START");
            break;
            case '3':  //讀電池電壓
            //read_voltage();
            break;
          }
        }
        //Serial.println(step_2_count);
        if(balance==-1)
        {
          step_funcion2();
        }
        if(balance==0)
        {
            /*for (int i = 0; i < TOTAL_IC; i++)
              for (int j = 0; j < CELL_NUM; j++)
                Serial.print(active_discharge_flag [i][j]);
            Serial.println( );*/
            //Serial.println("執行30秒");
            //read_voltage();
            polling_read();
            //******************************************************************************
            if(first==0)
            {
              polling_read();
              write_LTC3300();
              delay(50);
              polling_read();
              excute_LTC3300();
              delay(50);
              first=1;
              polling_read();
            }
            else
            {
              write_LTC3300();
              delay(50);
              polling_read();
            }
              read_count++;
            if(read_count==5)
            {
              read_count=0;
              for (int i = 0; i < TOTAL_IC; i++)
              {
                for (int j = 0; j < CELL_NUM; j++)
                {
                  if((float)cell_codes[i][j] * 0.0001>4.1||(float)cell_codes[i][j] * 0.0001<3.5)
                  {
                    polling_read();
                     active_discharge_flag [i][j]=0;
                     write_in_3300();
                     tx_comm_1[5] = 0X09;
                     tx_comm_2[3] = 0X08;
                     tx_comm_3[5] = 0X09;
                    //tx_comm_4[3] = 0X09;
                    polling_read();
                     change_information();
                     polling_read();
                     crc();
                     tx_comm_1[5] = (tx_comm_1[5] | (crc_new_logisim (CRC_1)) << 4) ;
                     tx_comm_2[3] = tx_comm_2[3] | (crc_new_logisim (CRC_2)) << 4;
                     tx_comm_3[5] = (tx_comm_3[5] | (crc_new_logisim (CRC_3)) << 4) ;
                     //tx_comm_4[3] = tx_comm_4[3] | (crc_new_logisim (CRC_4)) << 4;
                     write_LTC3300();
                     polling_read();
                      delay(50);
                      polling_read();
                     excute_LTC3300();
                      delay(50);
                      polling_read();
                  }
                  polling_read();
                }
              }
            }
            time_counter++;
            polling_read();
            start_balance_end_condition();
            polling_read();
            //*****************************************************************************
        }
        if(balance==1)
        {
          //Serial.println("暫停30秒");
            polling_read();
            first=0;
            delay(10);
            time_counter++;
            //read_voltage();
            //************************
            if(time_counter>=stop_balance||balance==-1)//30s
            {
              time_counter=0;
              balance=0;
              end_condition();
              polling_read();
              /*for (int i = 0; i < 4; i++)
                {
                  Serial.println(high_vlotage[i],4);
                  Serial.println(low_vlotage[i],4);
                }*/
              if((charge_discharge_flag==3)&&((high_vlotage[5]-low_vlotage[5])>=0.1))
              {
                for(int i=0;i<3;i++)
                  balance_flag[i]=0;
                for(int i=0;i<2;i++)
                  balance2_flag[i]=0;
                charge_discharge_flag=1;
                polling_read();
              }
              polling_read();
              if(charge_discharge_flag==1)
              {
                for(int i=0;i<3;i++)
                  balance_flag[i]=0;
                for (int i = 0; i < 3; i++)
                {
                  //Serial.println((high_vlotage[i]-low_vlotage[i]),4);
                  if((high_vlotage[i]-low_vlotage[i])<0.1)
                    balance_flag[i]=1;
                    polling_read();
                }
                /*for (int i = 0; i < 3; i++)
                    Serial.println(balance_flag[i]);*/
                if((balance_flag[0])&&(balance_flag[1])&&(balance_flag[2]))
                {
                  interrupt_LTC3300();
                  charge_discharge_flag++;
                  polling_read();
                }
                polling_read();
              }
              else if(charge_discharge_flag==2)
              {
                for(int i=0;i<2;i++)
                  balance2_flag[i]=0;
                step_2_count++;
                //Serial.println(step_2_count);
                polling_read();
                if(step_2_count>=6)
                {
                  polling_read();
                  step_2_count=0;
                  charge_discharge_flag=1;
                  for(int i=0;i<3;i++)
                  balance_flag[i]=0;
                  polling_read();
                   end_condition();
                }
                polling_read();
                for (int i = 3; i < 5; i++)
                {
                  //Serial.println((high_vlotage[i]-low_vlotage[i]),4);
                  if((high_vlotage[i]-low_vlotage[i])<0.1)
                    balance2_flag[i-3]=1;
                    polling_read();
                }
                 if((balance2_flag[0])&&(balance2_flag[1]))
                {
                  interrupt_LTC3300();
                  charge_discharge_flag=3;
                  polling_read();
                }
              }
              else if(charge_discharge_flag==3)
              {
                if((high_vlotage[5]-low_vlotage[5])<=0.1)
                {
                  interrupt_LTC3300();
                  charge_discharge_flag++;
                  polling_read();
                }
              }
              else if(charge_discharge_flag==4)
              {
                 charge_discharge_flag=1;
                 balance=4;
                 polling_read();
              }
              polling_read();
              detect_charde_discharge();
              polling_read();
              for (int i = 0; i < 6; i++)
              {
                if((high_vlotage[i]-low_vlotage[i])>=0.5)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=323;//120
                  else
                    start_balance[i]=648;//120
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.5&&(high_vlotage[i]-low_vlotage[i])>=0.4)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=270;//100
                  else
                    start_balance[i]=540;//100
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.4&&(high_vlotage[i]-low_vlotage[i])>=0.3)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=215;//80
                  else
                    start_balance[i]=432;//80
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.3&&(high_vlotage[i]-low_vlotage[i])>=0.2)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=162;//60
                  else
                    start_balance[i]=324;//60
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.2&&(high_vlotage[i]-low_vlotage[i])>=0.1)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=108;//40
                  else
                    start_balance[i]=216;//40
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.1&&(high_vlotage[i]-low_vlotage[i])>=0.05)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=54;//20 
                  else
                    start_balance[i]=108;//20 
                  stop_balance=2200;//30
                  polling_read();
                }
                else if((high_vlotage[i]-low_vlotage[i])<0.05&&(high_vlotage[i]-low_vlotage[i])>=0.01)
                {
                  if(charge_discharge_flag==1)
                    start_balance[i]=27;//10s
                  else
                    start_balance[i]=54;//10s
                  stop_balance=2200;//30s
                  polling_read();
                }
                else
                {
                  start_balance[i]=0;//15s
                  stop_balance=2200;//30s
                  polling_read();
                }
                polling_read();
              }
              polling_read();
              for (int i = 0; i < 6; i++)
              {
                high_vlotage[i]=0.0;
                low_vlotage[i]=5.0;
              }
              polling_read();
              /*for (int i = 0; i < 6; i++)
              {
                Serial.println(start_balance[i]);
              }*/
              //Serial.println(charge_discharge_flag);
            }
            //***************************
          }
}
  /*if (Serial.available())
  {
    char val = Serial.read();
    switch (val)
    {
      case '1':  //寫入暫存器
        wakeup_idle();
        LTC6804_wrcfg(TOTAL_IC, tx_cfg);
        print_information ();
        break;
      case '2':  //讀暫存器
        wakeup_idle();
        LTC6804_rdcfg(TOTAL_IC, re_cfg);
        print_information ();
        break;
      case '3':  //讀電池電壓
        read_voltage();
        break;
      case '4':  //讀GPIO腳位
        wakeup_idle();
        LTC6804_adax();
        error = LTC6804_rdaux(0, TOTAL_IC, GPIO_codes);
        if (error == 255)
          Serial.println("A PEC error was detected in the received data");
        else
          print_information ();
        break;
      case '5':  //寫入3300
        write_LTC3300();
        break;
      case '6':  //執行3300
        excute_LTC3300();
        break;
      case '7':  //中斷3300
        interrupt_LTC3300();
        break; 
      case '8':
        wakeup_idle();
        detect_charde_discharge();
        break;
      case '9':
      while(1)
      {
        read_voltage();
        //讀電壓
        
        if (Serial.available())
        {
          char val = Serial.read();
            
        }
          //偵測充放電
          wakeup_idle();
          detect_charde_discharge();
          //寫入LTC3300
          write_LTC3300();
         //執行充放電
          excute_LTC3300();
          //中斷3300
         interrupt_LTC3300();
        }
        break;
      }
    }*/
