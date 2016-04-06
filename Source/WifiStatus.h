#pragma once

#include "Utils.h"

struct WifiAccessPoint {
  String ssid;
  int signalStrength; // -120 to 0
  bool requiresAuth;
};

struct WifiStatus {
  OwnedArray<WifiAccessPoint> accessPoints;
  WifiAccessPoint *connectedAccessPoint = nullptr;
  bool enabled = false;
  bool connected = false;

  void setEnabled();
  void setDisabled();
  void setConnectedAccessPoint(WifiAccessPoint *ap, String psk = String());
  void setDisconnected();

  void populateFromJson(const var &json);
};
