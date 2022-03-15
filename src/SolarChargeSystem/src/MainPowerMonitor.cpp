/*
 * @Author: TZU-CHIEH,HSU
 * @Date: 2022-02-26 23:38:47
 * @LastEditors: TZU-CHIEH,HSU
 * @LastEditTime: 2022-03-15 01:37:55
 * @Description: 
 */
#include "MainPowerMonitor.h"

void MainPowerMonitor::begin()
{
    pinMode(MAINPWR_MON_HALL_REF_PIN,INPUT);
    pinMode(MAINPWR_MON_HALL_PIN,INPUT);
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
    refAdcVal = analogRead(MAINPWR_MON_HALL_REF_PIN);
}

float MainPowerMonitor::getCurrentA()
{
    return (currentAdcVal-refAdcVal) * MAINPWR_MON_VREF / 1024 * MAINPWR_MON_HALL_GAIN;
}

float MainPowerMonitor::getVoltage()
{
    return voltageAdcVal * MAINPWR_MON_VREF * MAINPWR_MON_V_GAIN / 1024;
}

void MainPowerMonitor::sampleCallback(SoftTimer& timer, void* arg)
{
    MainPowerMonitor *_this = (MainPowerMonitor*)arg;
    _this->voltageAdcVal = analogRead(MAINPWR_MON_V_PIN);
    _this->currentAdcVal = analogRead(MAINPWR_MON_HALL_PIN);

    _this->calibrationDivideTimes ++;
    if (_this->calibrationDivideTimes == HST016L_CALIBRATION_TIMES_DIVIDE)
        _this->calibration();
    

    //Serial.println(_this->getCurrentA());
}