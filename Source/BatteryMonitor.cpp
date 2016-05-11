#include "BatteryMonitor.h"
#include "Utils.h"

BatteryMonitor::BatteryMonitor( )
  : Thread( "BatteryMonitor" ) {
  status.percentage = 0;
  status.isCharging = 0;
  
  auto voltageFileName = absoluteFileFromPath( "/usr/lib/pocketchip-batt/voltage" );
  auto chargingFileName = absoluteFileFromPath( "/usr/lib/pocketchip-batt/charging" );
  
  voltageFile = File( voltageFileName );
  chargingFile = File( chargingFileName );
}

BatteryMonitor::~BatteryMonitor( ) {
}

const BatteryStatus& BatteryMonitor::getCurrentStatus( ) {
  return status;
}

void BatteryMonitor::updateStatus() {
  if( chargingFile.exists() ) {
    auto chargingValue = chargingFile.loadFileAsString();
    status.isCharging = chargingValue.getIntValue();
    //if(status.isCharging != 0) status.isCharging = 1;
  }
  
  if( voltageFile.exists() ) {
    auto voltageValue = voltageFile.loadFileAsString();
    
    float voltageOffset = (voltageValue.getFloatValue()*.001) - minVoltage;
    float maxOffset = maxVoltage - minVoltage;
    
    // turn voltage into a percentage we can use
    status.percentage = (voltageOffset * 100)/maxOffset;
    
    // only show lowest percentage graphic if battery is at least 15%
    if ( status.percentage <= 25 && status.percentage > 15) {
      status.percentage = 15;
    }
    // limit range to [0:100]
    if(status.percentage>100) status.percentage = 100;
    if(status.percentage<0) status.percentage = 0;
  }
}

void BatteryMonitor::run( ) {
  while( !threadShouldExit() ) {
    updateStatus();
    wait(2000);
  }
}