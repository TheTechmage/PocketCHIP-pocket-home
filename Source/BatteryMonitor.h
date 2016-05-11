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
  
  File voltageFile;
  File chargingFile;
  
  float maxVoltage = 4.25;
  float minVoltage = 3.275;
public:
  BatteryMonitor();
  ~BatteryMonitor();
  
  const BatteryStatus& getCurrentStatus();
  void updateStatus();
  
  virtual void run();
};