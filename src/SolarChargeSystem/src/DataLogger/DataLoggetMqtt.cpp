

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
    DataLoggerMQTT *_this = (DataLoggerMQTT *)arg;
    Serial.println(F("Start to Push all data !"));
    //_this->progressRestart();
    _this->publicJson();
}

/**
 * @brief Publish all data to MQTT Broker
 * @note  Method 1 (Without Json)
 */
void DataLoggerMQTT::publish()
{
    // ! Duplicated
    // if (progress == DataLoggerMQTTProgress::FINISH)
    //     return;

    // if (!LTE->isComplete())
    //     return;

    // const char *topic = NULL;

    // message[0] = '\0';
    // char str_temp[6];

    // switch (progress)
    // {
    // case DataLoggerMQTTProgress::MainBasic:
    // {
    //     progress = DataLoggerMQTTProgress::MPPT_Vin;
    //     topic = "MAIN/Basic";

    //     break;
    // }
    // case DataLoggerMQTTProgress::MPPT_Vin:
    // {
    //     progress = DataLoggerMQTTProgress::MPPT_Iin;
    //     topic = "MPPT/Vin";
    //     // strcpy(message,"0.0,1.1,2.2,3.3,4.4,5.5");
    //     dtostrf(mpptModule.valTemp[0][0], 4, 3, str_temp);
    //     sprintf(message, "%s", str_temp);
    //     Serial.println(message);
    //     break;
    // }
    // case DataLoggerMQTTProgress::MPPT_Iin:
    // {
    //     progress = DataLoggerMQTTProgress::MPPT_Vout;
    //     topic = "MPPT/Iin";
    //     dtostrf(mpptModule.valTemp[0][1], 4, 3, str_temp);
    //     sprintf(message, "%s", str_temp);
    //     // snprintf(message, strlen(message), "%f",  str_temp);
    //     break;
    // }
    // case DataLoggerMQTTProgress::MPPT_Vout:
    // {
    //     progress = DataLoggerMQTTProgress::MPPT_Iout;
    //     topic = "MPPT/Vout";
    //     dtostrf(mpptModule.valTemp[0][2], 4, 3, str_temp);
    //     sprintf(message, "%s", str_temp);
    //     break;
    // }
    // case DataLoggerMQTTProgress::MPPT_Iout:
    // {
    //     progress = DataLoggerMQTTProgress::MAIN_ILL;
    //     topic = "MPPT/Iout";
    //     dtostrf(mpptModule.valTemp[0][3], 4, 3, str_temp);
    //     sprintf(message, "%s", str_temp);

    //     break;
    // }
    // case DataLoggerMQTTProgress::MAIN_ILL:
    // {
    //     progress = DataLoggerMQTTProgress::MAIN_TEMP;
    //     topic = "MAIN/ILL";
    //     dtostrf(envSensor.getValue(0), 4, 3, str_temp);
    //     sprintf(message, "%s", str_temp);

    //     break;
    // }
    // case DataLoggerMQTTProgress::MAIN_TEMP:
    // {
    //     progress = DataLoggerMQTTProgress::FINISH;
    //     topic = "MAIN/TEMP";
    //     dtostrf(envSensor.getTemperature(), 4, 3, str_temp);
    //     sprintf(message, "%s", str_temp);

    //     Serial.println(F("Stop to Push all data !"));

    //     break;
    // }
    // }
    // if (topic != NULL && strlen(message) != 0)
    //     LTE->MQTT_publish(topic, message, strlen(message), 0, 0);
}

void DataLoggerMQTT::progressRestart()
{
    progress = DataLoggerMQTTProgress::MainBasic;
}

void DataLoggerMQTT::publicJson()
{
    const char *topic = "Drone1";
    StaticJsonDocument<400> doc;

    doc["IDR"] = 3.58;

    JsonObject ENV = doc.createNestedObject("ENV");
    JsonArray ENV_ILL = ENV.createNestedArray("ILL");
    ENV_ILL.add(envSensor.getValue(0));
    ENV_ILL.add(NAN);
    JsonArray ENV_AIR = ENV.createNestedArray("AIR");
    ENV_AIR.add(envSensor.getTemperature());
    ENV_AIR.add(NAN);

    JsonObject MPPT = doc.createNestedObject("MPPT");

    JsonArray MPPT_Vin = MPPT.createNestedArray("Vin");
    JsonArray MPPT_Iin = MPPT.createNestedArray("Iin");
    JsonArray MPPT_Vout = MPPT.createNestedArray("Vout");
    JsonArray MPPT_Iout = MPPT.createNestedArray("Iout");

    // for (int i=0;i<6;i++)
    // {
    //     MPPT_Vin.add(round3(mpptModule.valTemp[i][0]));
    //     MPPT_Iin.add(round3(mpptModule.valTemp[i][1]));
    //     MPPT_Vout.add(round3(mpptModule.valTemp[i][2]));
    //     MPPT_Iout.add(round3(mpptModule.valTemp[i][3]));
    // }

    for (int i=0;i<MPPT_DEVICES;i++)
    {
        MPPT_Vin.add(round3(mpptModule.getInputVoltage(i)));
        MPPT_Iin.add(round3(mpptModule.getInputCurrent(i)));
        MPPT_Vout.add(round3(mpptModule.getOutputVoltage(i)));
        MPPT_Iout.add(round3(mpptModule.getOutputCurrent(i)));
    }

    JsonObject BMS = doc.createNestedObject("BMS");

    JsonArray BMS_Vbat = BMS.createNestedArray("Vbat");
    BMS_Vbat.add(1.03);
    BMS_Vbat.add(2.25);
    BMS_Vbat.add(3.18);
    BMS_Vbat.add(9.87);
    BMS_Vbat.add(1);
    BMS_Vbat.add(0.32);

    JsonArray BMS_Mode = BMS.createNestedArray("Mode");
    BMS_Mode.add(3);
    BMS_Mode.add(2);
    BMS_Mode.add(1);
    BMS_Mode.add(0);
    BMS_Mode.add(1);
    BMS_Mode.add(2);

    char message[400] = "";
    serializeJson(doc, message);
    Serial.println(message);
    if (topic != NULL && strlen(message) != 0)
        LTE->MQTT_publish(topic, message, strlen(message), 0, 0);
}

double DataLoggerMQTT::round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

double DataLoggerMQTT::round3(double value) {
   return (int)(value * 1000 + 0.5) / 1000.0;
}