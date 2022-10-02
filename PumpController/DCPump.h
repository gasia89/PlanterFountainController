#ifndef DCPump_h
#define DCPump_h
#include <Arduino.h>
#include "FloatSwitch.h"
#include <iostream>
#include <string>


class FloatSwitch;

class DCPump {
  private:
    int Pin;
    bool Enabled;
    std::string PumpID; 
    int CurrentDutyCycle;
    FloatSwitch KillSwitch;
  
  public:
    int MinDutyCycle;
    DCPump();
    DCPump(int pin, FloatSwitch& killSwitch, bool enabled, std::string pumpID, int minDutyCycle, int initDutyCycle);
    void stopPump();
    bool startPumpIsSuccess();
    bool waterLevelIsGood();
    void setDutyCycle(int newDutyCycle);
    bool rampToDutyCycleIsSuccess(int newDutyCyclePercent, int timeToRampInSeconds);
    bool isEnabled();
    int getDutyCycle();
    std::string getName();
};
#endif
