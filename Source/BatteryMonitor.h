#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Utils.h"

struct BatteryStatus {
  int percentage;
  int isCharging;
};
class BatteryMonitor: public juce::Thread {
private:
  BatteryStatus status;
  float maxVoltage = 4.17;
  float minVoltage = 3.275;
public:
  BatteryMonitor();
  ~BatteryMonitor();
  
  virtual void run();
  
  const BatteryStatus& getCurrentStatus();
  
};