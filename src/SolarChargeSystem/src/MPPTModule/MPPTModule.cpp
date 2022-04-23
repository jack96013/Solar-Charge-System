#include "MPPTModule.h"

void MPPTModule::begin()
{
    configuration.mux = ADS1119MuxConfiguration::positiveAIN0negativeAGND;
    configuration.gain = ADS1119Configuration::Gain::one;
    configuration.dataRate = ADS1119Configuration::DataRate::sps20;
    configuration.conversionMode = ADS1119Configuration::ConversionMode::singleShot;
    configuration.voltageReference = ADS1119Configuration::VoltageReferenceSource::internal;

    
    calibration.gain = ADS1119Configuration::Gain::one;
    calibration.dataRate = ADS1119Configuration::DataRate::sps20;
    calibration.conversionMode = ADS1119Configuration::ConversionMode::continuous;
    calibration.voltageReference = ADS1119Configuration::VoltageReferenceSource::internal;

    adc.begin();
    for (int i = 0; i < MPPT_DEVICES; i++)
    {
        uint8_t addr = address[i];
        if (i2cManager.isAvailable(addr))
        {
            adc.setAddress(addr);
            adc.reset();

            calibration.mux = ADS1119MuxConfiguration::positiveAIN0negativeAGND; // VREF connected to AIN2 !!!
            adc.performOffsetCalibration(calibration);
            calibration.mux = ADS1119MuxConfiguration::positiveAIN1negativeAGND; // VREF connected to AIN2 !!!
            adc.performOffsetCalibration(calibration);
            calibration.mux = ADS1119MuxConfiguration::positiveAIN2negativeAGND; // VREF connected to AIN2 !!!
            adc.performOffsetCalibration(calibration);
            calibration.mux = ADS1119MuxConfiguration::positiveAIN3negativeAGND; // VREF connected to AIN2 !!!
            adc.performOffsetCalibration(calibration);
            adc.writeConfig(configuration);
        }
        zeroCurrentVoltage[i][0] = MPPT_I_ZCO;
        zeroCurrentVoltage[i][1] = MPPT_I_ZCO;
    }

    // module2.begin();
    // module2.reset();

    getAdcTimer.setOnExpiredCallback(getAdcCallback, this);
    getAdcTimer.setInterval(1000 / 4);
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
    for (uint8_t i = startAddress; i < endAddress; i++)
    {
    }
    MPPT_LOGLN(F("Scan finish"));
}

void MPPTModule::getAdcCallback(SoftTimer &timer, void *arg)
{

    MPPTModule *_this = (MPPTModule *)arg;

    for (int index = 0; index < MPPT_DEVICES; index++)
    {
        //_this->readMpptModule(i);
        _this->adc.setAddress(_this->address[index]);
        float volt = _this->adc.readVoltageDirectly(_this->configuration);
        float diff;
        switch (_this->currentChannel)
        {

        case MPPT_VIN:
            _this->valTemp[index][MPPT_VIN_INDEX] = volt * MPPT_VIN_GAIN;
            break;
        case MPPT_IIN:
            diff = volt * MPPT_I_GAIN_DIV - _this->zeroCurrentVoltage[index][0];
            _this->valTemp[index][MPPT_IIN_INDEX] = diff * MPPT_I_GAIN_SEN;
            if (_this->onCalibrate)
                _this->zeroCurrentVoltage[index][0] = volt * MPPT_I_GAIN_DIV;
            break;
        case MPPT_IOUT:

            diff = volt * MPPT_I_GAIN_DIV - _this->zeroCurrentVoltage[index][1];
            _this->valTemp[index][MPPT_IOUT_INDEX] = diff * MPPT_I_GAIN_SEN;

            if (_this->onCalibrate)
            {
                _this->zeroCurrentVoltage[index][1] = volt * MPPT_I_GAIN_DIV;
                if (index == MPPT_DEVICES - 1)
                    _this->onCalibrate = false;
            }
            break;
        case MPPT_VOUT:
            _this->valTemp[index][MPPT_VOUT_INDEX] = volt * MPPT_VOUT_GAIN;
            break;
        }
    }
    // Switch Channel
    _this->currentChannel++;
    if (_this->currentChannel == 4)
        _this->currentChannel = 0;
    _this->configuration.mux = (ADS1119MuxConfiguration)(_this->currentChannel + 0b011);

    for (int i = 0; i < MPPT_DEVICES; i++)
    {

        _this->adc.setAddress(_this->address[i]);
        Serial.println("Start");
        _this->adc.writeConfig(_this->configuration);
        Serial.println("End");
        // delay(10);
    }
}

void MPPTModule::calibrate()
{
    onCalibrate = true;
    return;
    for (int i = 0; i < MPPT_DEVICES; i++)
    {
        uint8_t addr = address[i];
        if (!i2cManager.isAvailable(addr))
            continue;
        zeroCurrentVoltage[i][0] = readVoltage(i, 1) * MPPT_I_GAIN_DIV;
        zeroCurrentVoltage[i][1] = readVoltage(i, 2) * MPPT_I_GAIN_DIV;
    }

    //     configuration.mux = ADS1119MuxConfiguration::positiveAIN0negativeAGND;
    // module2_zco[0] = module2.readVoltage(configuration) * MPPT_I_GAIN_DIV;

    // configuration.mux = ADS1119MuxConfiguration::positiveAIN3negativeAGND;
    // module2_zco[1] = module2.readVoltage(configuration) * MPPT_I_GAIN_DIV;
}

float MPPTModule::readVoltage(uint8_t index, uint8_t channel)
{
    uint8_t addr = address[index];
    if (!i2cManager.isAvailable(addr))
        return NAN;
    adc.setAddress(addr);
    // Single Ended Mode
    configuration.mux = (ADS1119MuxConfiguration)(channel + 0b011);
    return adc.readVoltage(configuration);
}

void MPPTModule::saveToTemp(uint8_t index, float Vin, float Iin, float Vout, float Iout)
{
    valTemp[index][MPPT_VIN_INDEX] = Vin;
    valTemp[index][MPPT_IIN_INDEX] = Iin;
    valTemp[index][MPPT_VOUT_INDEX] = Vout;
    valTemp[index][MPPT_IOUT_INDEX] = Iout;
}

void MPPTModule::readMpptModule(uint8_t index)
{
    float volt;
    float Vin, Iin, Vout, Iout;

    volt = readVoltage(index, MPPT_IIN_INDEX);
    Iin = (volt * MPPT_I_GAIN_DIV - zeroCurrentVoltage[index][0]) * MPPT_I_GAIN_SEN;
    volt = readVoltage(index, MPPT_VIN_INDEX);
    Vin = volt * MPPT_VIN_GAIN;
    volt = readVoltage(index, MPPT_IOUT_INDEX);
    Iout = (volt * MPPT_I_GAIN_DIV - zeroCurrentVoltage[index][1]) * MPPT_I_GAIN_SEN;
    volt = readVoltage(index, MPPT_VOUT_INDEX);
    Vout = volt * MPPT_VOUT_GAIN;
    saveToTemp(index, Vin, Iin, Vout, Iout);
}

float MPPTModule::getInputVoltage(uint8_t index)
{
    if (index >= MPPT_DEVICES || !i2cManager.isAvailable(address[index]))
        return NAN;
    return valTemp[index][MPPT_VIN_INDEX];
}
float MPPTModule::getInputCurrent(uint8_t index)
{
    if (index >= MPPT_DEVICES || !i2cManager.isAvailable(address[index]))
        return NAN;
    return valTemp[index][MPPT_IIN_INDEX];
}
float MPPTModule::getOutputVoltage(uint8_t index)
{
    if (index >= MPPT_DEVICES || !i2cManager.isAvailable(address[index]))
        return NAN;
    return valTemp[index][MPPT_VOUT_INDEX];
}
float MPPTModule::getOutputCurrent(uint8_t index)
{
    if (index >= MPPT_DEVICES || !i2cManager.isAvailable(address[index]))
        return NAN;
    return valTemp[index][MPPT_IOUT_INDEX];
}