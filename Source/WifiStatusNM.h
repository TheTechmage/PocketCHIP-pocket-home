#pragma once

#include "Utils.h"
#include "WifiStatus.h"

class WifiStatusNM : public WifiStatus {
public:
  WifiStatusNM();
  ~WifiStatusNM();
  
  OwnedArray<WifiAccessPoint> accessPoints;
  WifiAccessPoint *connectedAccessPoint = nullptr;
  bool enabled = false;
  bool connected = false;

  void addListener(Listener* listener) override;

  void setEnabled() override;
  void setDisabled() override;
  void setConnectedAccessPoint(WifiAccessPoint *ap, String psk = String::empty) override;
  void setDisconnected() override;

  void initializeStatus() override;

private:
  Array<Listener*> listeners;
};
