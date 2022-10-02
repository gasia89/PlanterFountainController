#include "FloatSwitch.h"
#include <iostream>
#include <string>
using namespace std;

FloatSwitch::FloatSwitch(){
}

FloatSwitch::FloatSwitch(int pin, string switchID){
  Pin = pin;
  State = false;
  SwitchID = switchID;
  pinMode(Pin, OUTPUT);
}

string FloatSwitch::getSwitchName(){
  return SwitchID;
}
bool FloatSwitch::getSwitchState(){
  State = digitalRead(Pin);

  if (State){
    std::cout << SwitchID << " is triggered!";
  }
  
  return State;
}
