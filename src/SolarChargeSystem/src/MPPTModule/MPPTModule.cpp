#include "MPPTModule.h"

void MPPTModule::begin()
{
    configuration.mux = ADS1119MuxConfiguration::positiveAIN0negativeAGND;
    configuration.gain = ADS1119Configuration::Gain::one;
    configuration.dataRate = ADS1119Configuration::DataRate::sps20;
    configuration.conversionMode = ADS1119Configuration::ConversionMode::continuous;
    configuration.voltageReference = ADS1119Configuration::VoltageReferenceSource::internal;


    module1.begin();
    module1.reset();

    getAdcTimer.setOnExpiredCallback(getAdcCallback, this);
    getAdcTimer.setInterval(100);
    getAdcTimer.start();

}

void MPPTModule::run()
{
    getAdcTimer.run();
}


void MPPTModule::scanDevices()
{
    MPPT_LOGLN(F("Scanning Devices..."));
    uint8_t startAddress = MPPT_ADDRESS_START;
    uint8_t endAddress = startAddress + MPPT_DEVICES;
    for(uint8_t i=startAddress;i < endAddress;i++)
    {
        
    }
    MPPT_LOGLN(F("Scan finish"));
}

void MPPTModule::getAdcCallback(SoftTimer &timer,void *arg)
{
    MPPTModule* _this = (MPPTModule*)arg;
    _this->configuration.mux = ADS1119MuxConfiguration::positiveAIN0negativeAGND;
    float val1 = _this->module1.readVoltage(_this->configuration);
    float current1 = (val1 * MPPT_I_GAIN - 2.52f) * 2.2222f;
    _this->configuration.mux = ADS1119MuxConfiguration::positiveAIN1negativeGND;
    float val2 = _this->module1.readVoltage(_this->configuration);
    float voltage2 = val2 * MPPT_VIN_GAIN;

    _this->configuration.mux = ADS1119MuxConfiguration::positiveAIN3negativeAGND;
    float val3 = _this->module1.readVoltage(_this->configuration);
    float current3 = (val3 * MPPT_I_GAIN - 2.52f) * 2.2222f;
    _this->configuration.mux = ADS1119MuxConfiguration::positiveAIN2negativeAGND;
    float val4 = _this->module1.readVoltage(_this->configuration);
    float voltage4 = val4 * MPPT_VOUT_GAIN;

    Serial.print(F("ADS Ii :"));
    Serial.print(current1,4);
    Serial.print(F("\tVi :"));
    Serial.print(voltage2,4);
    Serial.print(F("\tIo :"));
    Serial.print(current3,4);
    Serial.print(F("\tVo :"));
    Serial.print(voltage4,4);
    Serial.println();
}