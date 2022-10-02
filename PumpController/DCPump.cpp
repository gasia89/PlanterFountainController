#include "DCPump.h"
#include "FloatSwitch.h"
#include <iostream>
#include <string>
using namespace std;

DCPump::DCPump(){}
DCPump::DCPump(int pin, FloatSwitch& killSwitch, bool enabled, string pumpID, int minDutyCycle = 30, int initDutyCycle = 0)
{
  Pin = pin;
  KillSwitch = killSwitch;
  Enabled = enabled;
  PumpID = pumpID;
  MinDutyCycle = minDutyCycle;
  CurrentDutyCycle = initDutyCycle;
  pinMode(Pin, OUTPUT);
}

string DCPump::getName(){
  return PumpID;
}

int DCPump::getDutyCycle(){
  return CurrentDutyCycle;
}

bool DCPump::isEnabled(){
  return Enabled;
}

void DCPump::stopPump(){
  std::cout << "Checking to see if pump state is stoppable";
  string enabledText = Enabled ? "Enabled" : "Disabled";
  std::cout << "Pump is " << enabledText;
  
  if (Enabled == true){
    Enabled = false;
    digitalWrite(Pin, 0);
  }
}

bool DCPump::waterLevelIsGood(){
  bool killSwitchIsTriggered = KillSwitch.getSwitchState();
  
  if (killSwitchIsTriggered){
    this->stopPump();
    return false;
  }
  else{
    return true;
  }
}

bool DCPump::startPumpIsSuccess(){
  bool killSwitchIsTriggered = KillSwitch.getSwitchState();
  // we always want to make sure there is water in the basin before starting the pump.
  if (waterLevelIsGood()){
    if (Enabled == false){
      Enabled = false;
      std::cout << "Starting " << PumpID << " at " << CurrentDutyCycle << "%";
      digitalWrite(Pin, CurrentDutyCycle);
      return true;
    }
    else{
      std::cout << "Pump is already running.";
    }
  }

  std::cout << "Low water level alarm triggered for " << PumpID;
  std::cout << "Pump Start Failed";

  return false;
}

void DCPump::setDutyCycle(int newDutyCycle){
  int convertedDutyCycle = (newDutyCycle/100)*255;
  if (convertedDutyCycle > 255){
    std::cout << "Supplied DutyCycle value of " << newDutyCycle << "% exceeds maximum DutyCycle. Setting value to 100%, which is as fast as I can go!";
    CurrentDutyCycle == 255;
  }
  else if (convertedDutyCycle >= 0 && convertedDutyCycle != CurrentDutyCycle){
    if (convertedDutyCycle < MinDutyCycle){
      std::cout << "Supplied DutyCycle value of " << newDutyCycle << "% is under the minimum DutyCycle. All engines stop.";
      CurrentDutyCycle = 0;
    }
    else if (convertedDutyCycle >= MinDutyCycle){
      std::cout << "Aye Cap\'n! " << newDutyCycle << "%, Aye!";
      CurrentDutyCycle = convertedDutyCycle;
    }
  }
}

bool DCPump::rampToDutyCycleIsSuccess(int newDutyCyclePercent, int timeToRampInSeconds){
  int offsetPerSecond = 0;
  int convertedDutyCycle = (newDutyCyclePercent/100)*255;
  int currentOffsetDutyCycle;
  int currentDutyPercent;
  int isSuccess = true; 

  if (convertedDutyCycle > CurrentDutyCycle){
    // ramp down
    std::cout << "Ramping down " << PumpID << " by " << newDutyCyclePercent << "% over the next " << timeToRampInSeconds << " seconds.";
    offsetPerSecond = convertedDutyCycle/timeToRampInSeconds;

    while (CurrentDutyCycle > convertedDutyCycle || isSuccess == false){
      currentOffsetDutyCycle = CurrentDutyCycle - offsetPerSecond;
      currentDutyPercent = (currentOffsetDutyCycle/255)*100;
      this->setDutyCycle(currentDutyPercent);
      isSuccess = this->startPumpIsSuccess();
    }

    std::cout << "Ramp up operation complete for " << PumpID;
  }
  else if (convertedDutyCycle < CurrentDutyCycle){
    // ramp up
    std::cout << "Ramping up " << PumpID << " by " << newDutyCyclePercent << "% over the next " << timeToRampInSeconds << " seconds.";
    offsetPerSecond = convertedDutyCycle/timeToRampInSeconds;

    while (CurrentDutyCycle < convertedDutyCycle || isSuccess == false){
      currentOffsetDutyCycle = CurrentDutyCycle + offsetPerSecond;
      currentDutyPercent = (currentOffsetDutyCycle/255)*100;
      this->setDutyCycle(currentDutyPercent);
      isSuccess = this->startPumpIsSuccess();
    }

    std::cout << "Ramp down operation complete for " << PumpID;
  }

  return isSuccess;
}
