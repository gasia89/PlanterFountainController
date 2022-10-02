#include "DCPump.h"
#include "FloatSwitch.h"
#include <iostream>

FloatSwitch pumpSwitch(22, "PumpSwitch");
FloatSwitch foggerSwitch(24, "FoggerSwitch");
DCPump waterfallPump(12, pumpSwitch, false, "WaterfallPump", 30, 0);
DCPump foggerPump(14, foggerSwitch, false, "FoggerPump", 30, 0);
/* 
 *  This value represents the amount of time to wait after turning on the waterfall pump for the first time
 *  before attempting to check the float switch. Tuning this value will will reduce wear on
 *  the pump in the event that there was not enough water in the basin.
*/
int primeDelayInMS = 8000;

// min duty cycle is 30%, max is 100%
int maxWaterfallFlowRate = 80;

// min duty cycle is 30%, max is 100%
int maxFoggerFlowRate = 100;

// the amount of time in seconds to ramp up to the max waterfall flow rate
int defaultWaterfallRampDuration = 15;
int defaultFoggerRampDuration = 5;

// if at any point this is set to true, we want to shut everything down to save the pumps.
bool waterLevelAlertThrown = false;

void setup(){
  Serial.begin(9600);

  // ensure both pumps are in an off state to start.
  waterfallPump.stopPump();
  foggerPump.stopPump();
}

bool fountainInit(){
  // check the level of the water in the basin before trying to start the pump
  waterLevelAlertThrown = !waterfallPump.waterLevelIsGood();
  
  if (waterLevelAlertThrown == false){
    // water level is good, set the pump to the lowest duty cycle possible to prime the pump
    waterfallPump.setDutyCycle(30);
    waterLevelAlertThrown = !waterfallPump.startPumpIsSuccess();
    
    // ensure the pump has a couple seconds to prime the tube and such.
    delay(primeDelayInMS);

    // check the float switch again now that its primed and make sure there is still enough water to keep going.
    waterLevelAlertThrown = !waterfallPump.waterLevelIsGood();
    
    if (waterLevelAlertThrown == false){
      bool foggerWaterIsLow = false;
      // ramp up the waterfall to configured max over configured duration.
      waterLevelAlertThrown = !waterfallPump.rampToDutyCycleIsSuccess(maxWaterfallFlowRate, defaultWaterfallRampDuration);

      while (waterLevelAlertThrown == false && foggerPump.isEnabled() == false){
        // with the waterfall in full swing, time to start the fogger
        foggerWaterIsLow = !foggerPump.waterLevelIsGood();
        if (foggerWaterIsLow == false){
           foggerPump.setDutyCycle(30);
           foggerWaterIsLow == !foggerPump.startPumpIsSuccess();
           delay(primeDelayInMS);
           waterLevelAlertThrown = !waterfallPump.waterLevelIsGood();
           
           if (foggerWaterIsLow == false && waterLevelAlertThrown == false){
            foggerWaterIsLow = !foggerPump.rampToDutyCycleIsSuccess(maxFoggerFlowRate, defaultFoggerRampDuration);
           }
        }

        waterLevelAlertThrown = !waterfallPump.waterLevelIsGood();
      }

      if (waterLevelAlertThrown){
        waterfallPump.stopPump();
        foggerPump.stopPump();
      }
    }
  }
}

void loop()
{
  if (waterfallPump.isEnabled() == false && foggerPump.isEnabled() == false){
    fountainInit();
  }
  else{
    // in this state, the fountain is running.
    // periodically check the water level to ensure it doesnt run dry.
    waterLevelAlertThrown = !waterfallPump.waterLevelIsGood();

    if (waterLevelAlertThrown){
      waterfallPump.stopPump();
      foggerPump.stopPump();
    }
  }
}
