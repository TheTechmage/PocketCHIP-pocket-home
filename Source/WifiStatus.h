#pragma once

#include "Utils.h"

struct WifiAccessPoint {
  String ssid;
  int signalStrength; // -120 to 0
  bool requiresAuth;
};

class WifiStatusListener {
public:
  WifiStatusListener();
  virtual ~WifiStatusListener();

  virtual void handleWifiEnabled() {};
  virtual void handleWifiDisabled() {};
  virtual void handleWifiConnected() {};
  virtual void handleWifiDisconnected() {};
};

class WifiStatus {
public:
  WifiStatus();
  ~WifiStatus();
  
  OwnedArray<WifiAccessPoint> accessPoints;
  WifiAccessPoint *connectedAccessPoint = nullptr;
  bool enabled = false;
  bool connected = false;

  void addListener(WifiStatusListener* listener);
  void setEnabled();
  void setDisabled();
  void setConnectedAccessPoint(WifiAccessPoint *ap, String psk = String());
  void setDisconnected();

  void populateFromJson(const var &json);
private:
  WifiStatusListener* listener = nullptr;
};
