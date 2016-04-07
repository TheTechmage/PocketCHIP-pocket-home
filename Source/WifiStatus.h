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
  ~WifiStatus();
  
  class Listener;
  
  OwnedArray<WifiAccessPoint> accessPoints;
  WifiAccessPoint *connectedAccessPoint = nullptr;
  bool enabled = false;
  bool connected = false;

  void addListener(Listener* listener);
  void setEnabled();
  void setDisabled();
  void setConnectedAccessPoint(WifiAccessPoint *ap, String psk = String());
  void setDisconnected();

  void populateFromJson(const var &json);
private:
  Array<Listener*> listeners;
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
