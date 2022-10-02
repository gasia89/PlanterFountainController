#ifndef FloatSwitch_h
#define FloatSwitch_h
#include <Arduino.h>
#include <iostream>
#include <string>

class FloatSwitch {
  private:
    int Pin;
    bool State;
    std::string SwitchID;
  
  public:
    FloatSwitch();
    FloatSwitch(int pin, std::string switchID);
    bool getSwitchState();
    std::string getSwitchName();
};
#endif
