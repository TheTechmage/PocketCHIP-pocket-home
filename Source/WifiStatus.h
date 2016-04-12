#pragma once

#include "Utils.h"

struct WifiAccessPoint {
  String ssid;
  int signalStrength; // -120 to 0
  bool requiresAuth;
};

class WifiStatus {
public:
  WifiStatus();
  virtual ~WifiStatus();
  
  class Listener;
  
  virtual OwnedArray<WifiAccessPoint> *nearbyAccessPoints() = 0;
  virtual WifiAccessPoint *connectedAccessPoint() const = 0;
  virtual bool isEnabled() const = 0;
  virtual bool isConnected() const = 0;

  virtual void addListener(Listener* listener) = 0;

  virtual void setEnabled() = 0;
  virtual void setDisabled() = 0;
  virtual void setConnectedAccessPoint(WifiAccessPoint *ap, String psk = String::empty) = 0;
  virtual void setDisconnected() = 0;

  virtual void initializeStatus() = 0;
};

class WifiStatus::Listener {
public:
  Listener();
  virtual ~Listener();
  
  virtual void handleWifiEnabled() {}
  virtual void handleWifiDisabled() {}
  virtual void handleWifiConnected() {}
  virtual void handleWifiDisconnected() {}
  virtual void handleWifiFailedConnect() {}
};

#include "WifiStatusJson.h"
#include "WifiStatusNM.h"
