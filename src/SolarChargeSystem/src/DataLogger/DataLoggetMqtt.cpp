

#include "DataLoggerMQTT.h"

void DataLoggerMQTT::begin()
{
    publishTimer.setOnExpiredCallback(publishCallback, this);
    publishTimer.setInterval(5000);
    publishTimer.start();
    LTE = lteManager.getLTEInstance();
}

void DataLoggerMQTT::run()
{
    if (!lteManager.isMQTTConnected())
        return;
    publishTimer.run();
    publish();
}

void DataLoggerMQTT::send()
{
}

void DataLoggerMQTT::publishCallback(SoftTimer &timer, void *arg)
{
    DataLoggerMQTT* _this = (DataLoggerMQTT*)arg;
    Serial.println(F("Start to Push all data !"));
    _this->progressRestart();
    
}

void DataLoggerMQTT::publish()
{
    if (progress == DataLoggerMQTTProgress::FINISH)
        return;

    if (!LTE->isComplete())
        return;

    const char *topic = NULL;

    message[0] = '\0';
    char str_temp[6];

    

    switch (progress)
    {
    case DataLoggerMQTTProgress::MainBasic:
    {
        progress = DataLoggerMQTTProgress::MPPT_Vin;
        topic = "MAIN/Basic";

        break;
    }
    case DataLoggerMQTTProgress::MPPT_Vin:
    {
        progress = DataLoggerMQTTProgress::MPPT_Iin;
        topic = "MPPT/Vin";
        //strcpy(message,"0.0,1.1,2.2,3.3,4.4,5.5");
        dtostrf(mpptModule.valTemp[0], 4, 3, str_temp);
        sprintf(message,"%s",str_temp);
        Serial.println(message);
        break;
    }
    case DataLoggerMQTTProgress::MPPT_Iin:
    {
        progress = DataLoggerMQTTProgress::MPPT_Vout;
        topic = "MPPT/Iin";
        dtostrf(mpptModule.valTemp[1], 4, 3, str_temp);
        sprintf(message,"%s",str_temp);
        //snprintf(message, strlen(message), "%f",  str_temp);
        break;
    }
    case DataLoggerMQTTProgress::MPPT_Vout:
    {
        progress = DataLoggerMQTTProgress::MPPT_Iout;
        topic = "MPPT/Vout";
        dtostrf(mpptModule.valTemp[2], 4, 3, str_temp);
        sprintf(message,"%s",str_temp);
        break;
    }
    case DataLoggerMQTTProgress::MPPT_Iout:
    {
        progress = DataLoggerMQTTProgress::MAIN_ILL;
        topic = "MPPT/Iout";
        dtostrf(mpptModule.valTemp[3], 4, 3, str_temp);
        sprintf(message,"%s",str_temp);
        
        break;
    }
    case DataLoggerMQTTProgress::MAIN_ILL:
    {
        progress = DataLoggerMQTTProgress::FINISH;
        topic = "MAIN/ILL";
        dtostrf(lightSensor.getValue(0), 4, 3, str_temp);
        sprintf(message,"%s",str_temp);

        Serial.println(F("Stop to Push all data !"));
        
        break;
    }
    }
    if (topic != NULL && strlen(message) != 0)
        LTE->MQTT_publish(topic, message, strlen(message), 0, 0);
}

void DataLoggerMQTT::progressRestart()
{
    progress = DataLoggerMQTTProgress::MainBasic;
}