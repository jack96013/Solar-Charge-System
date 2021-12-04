#include <Arduino.h>
#include "SerialManager.h"
#include "BatteryBalance.h"

SerialManager serialManager;    // Serial 相關
BatteryBalance batteryBalance;  


void setup()
{
  //serialManager.begin();
  batteryBalance.begin(); // 電池平衡
}

void loop()
{
  //serialManager.run();
  batteryBalance.run();

}