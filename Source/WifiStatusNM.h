#pragma once

#ifdef LINUX

#include <NetworkManager.h>

#include "Utils.h"
#include "WifiStatus.h"

class NMListener;

class WifiStatusNM : public WifiStatus {
public:
  WifiStatusNM();
  ~WifiStatusNM() override;
  
  OwnedArray<WifiAccessPoint> nearbyAccessPoints() override;
  WifiAccessPoint connectedAccessPoint() const override;
  bool isEnabled() const override;
  bool isConnected() const override;

  void addListener(Listener* listener) override;
  void clearListeners() override;

  void setEnabled() override;
  void setDisabled() override;
  void setConnectedAccessPoint(WifiAccessPoint *ap, String psk = String::empty) override;
  void setDisconnected() override;

  void initializeStatus() override;

  void handleWirelessEnabled();
  void handleWirelessConnected();
  void handleConnectedAccessPoint();

private:
  Array<Listener*> listeners;
  ScopedPointer<WifiAccessPoint> connectedAP = nullptr;
  bool enabled = false;
  bool connected = false;
  bool connecting = false;

  NMClient* connectToNetworkManager();

  NMClient *nmclient = nullptr;
  NMDevice *nmdevice = nullptr;

  ScopedPointer<NMListener> nmlistener = nullptr;
};


class NMListener : public Thread {
public:
  NMListener();
  ~NMListener();

  void initialize(WifiStatusNM* status, NMClient *client);
  void run() override;
private:
  GMainLoop *loop;
  GMainContext *context;
  NMClient *nm;
  WifiStatusNM *wifiStatus;
};

#endif // LINUX
