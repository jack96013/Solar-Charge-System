/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-04-10 02:43:44
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-04-12 06:50:39
 * @Description: 
 */
#ifndef __DATALOGGERMQTT_H__
#define __DATALOGGERMQTT_H__

#include <Arduino.h>
#include "LTEManager.h"
#include "SoftTimer.h"
#include "AsyncLTE.h"
#include "LightSensor.h"

#include "MainPowerMonitor.h"
#include "MPPTModule/MPPTModule.h"

extern LTEManager lteManager;
extern MPPTModule mpptModule;
extern LightSensor lightSensor;

#define DATALOGGERMQTT_MESSAGE_LEN 70

class DataLoggerMQTT
{
    public:
        void begin();
        void run();
        void send();
  
        void publish();

    private:
        void* requestSequence[5] = {NULL};
        uint8_t mppt_device_index;
        // 依照
        SoftTimer publishTimer;
        AsyncLTE* LTE = NULL;
        static void publishCallback(SoftTimer &timer, void *arg);

        enum class DataLoggerMQTTProgress
        {
            FINISH,
            MainBasic,
            MPPT_Vin,
            MPPT_Iin,
            MPPT_Vout,
            MPPT_Iout,
            BMS_VBat,
            BMS_MODE,
            MAIN_ILL,
        };
        DataLoggerMQTTProgress progress;

        void nextProcess();
        void progressRestart();
        char message[DATALOGGERMQTT_MESSAGE_LEN] = "";
        
};
#endif