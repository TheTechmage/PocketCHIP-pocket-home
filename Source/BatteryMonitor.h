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
  float maxVoltage = 4.25;
  float minVoltage = 3.275;
public:
  BatteryMonitor();
  ~BatteryMonitor();
  
  virtual void run();
  
  // FIXME: this class is not thread safe. Only works because we're on
  // a single threaded machine.
  const BatteryStatus& getCurrentStatus();
  
};