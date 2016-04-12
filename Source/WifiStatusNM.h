#pragma once

#include "Utils.h"
#include "WifiStatus.h"

class WifiStatusNM : public WifiStatus {
public:
  WifiStatusNM();
  ~WifiStatusNM() override;
  
  OwnedArray<WifiAccessPoint> *nearbyAccessPoints() override;
  WifiAccessPoint *connectedAccessPoint() const override;
  bool isEnabled() const override;
  bool isConnected() const override;

  void addListener(Listener* listener) override;

  void setEnabled() override;
  void setDisabled() override;
  void setConnectedAccessPoint(WifiAccessPoint *ap, String psk = String::empty) override;
  void setDisconnected() override;

  void initializeStatus() override;

private:
  Array<Listener*> listeners;
  OwnedArray<WifiAccessPoint> accessPoints;
  WifiAccessPoint *connectedAP = nullptr;
  bool enabled = false;
  bool connected = false;
};
