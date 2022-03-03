#include "MainPowerMonitor.h"

void MainPowerMonitor::begin()
{
    pinMode(HST016L_REF_PIN,INPUT);
    pinMode(HST016L_VOUT_PIN,INPUT);
    sampleTimer.start();
    calibration();
}

void MainPowerMonitor::run()
{
    sampleTimer.run();
}

float MainPowerMonitor::getPowerWatt()
{
    return getVoltage() * getCurrentA();
}

void MainPowerMonitor::calibration()
{
    refAdcVal = analogRead(HST016L_REF_PIN);
}

float MainPowerMonitor::getCurrentA()
{
    return (currentAdcVal-refAdcVal) * MAINPOWER_ADC_REF * HST016L_GAIN_MV_PER_A / 1024;
}

float MainPowerMonitor::getVoltage()
{
    return voltageAdcVal * MAINPOWER_ADC_REF * MAINPOWER_VIN_GAIN / 1024;
}

void MainPowerMonitor::sampleCallback(SoftTimer& timer, void* arg)
{
    MainPowerMonitor *_this = (MainPowerMonitor*)arg;
    _this->voltageAdcVal = analogRead(MAINPOWER_VIN_PIN);
    _this->currentAdcVal = analogRead(HST016L_VOUT_PIN);
    _this->calibrationDivideTimes ++;
    if (_this->calibrationDivideTimes == HST016L_CALIBRATION_TIMES_DIVIDE)
        _this->calibration();

    //Serial.println(_this->getVoltage());
    //Serial.println(_this->getCurrentA());
}