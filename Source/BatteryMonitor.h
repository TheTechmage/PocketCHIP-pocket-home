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
public:
  BatteryMonitor();
  ~BatteryMonitor();
  
  virtual void run();
  
  const BatteryStatus& getCurrentStatus();
  
};