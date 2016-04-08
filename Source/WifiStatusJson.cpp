#include <map>

#include "Utils.h"

#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

WifiStatusJson::WifiStatusJson() : listeners() {}
WifiStatusJson::~WifiStatusJson() {}

OwnedArray<WifiAccessPoint> *WifiStatusJson::nearbyAccessPoints() {
  return &accessPoints;
}

WifiAccessPoint *WifiStatusJson::connectedAccessPoint() const {
  return connectedAP;
}

bool WifiStatusJson::isEnabled() const {
  return enabled;
}

bool WifiStatusJson::isConnected() const {
  return connected;
}

void WifiStatusJson::addListener(Listener* listener) {
  listeners.add(listener);
}

// TODO: direct action should not be named set, e.g. enable/disable/disconnect
// otherwise easily confused with setters thats wrap members, which are slightly different idiom
void WifiStatusJson::setEnabled() {
  if (!enabled) {
    enabled = true;
    for(const auto& listener : listeners) {
      listener->handleWifiEnabled();
    }
  }
}

void WifiStatusJson::setDisabled() {
  if (enabled) {
    enabled = false;
    for(const auto& listener : listeners) {
      listener->handleWifiDisabled();
    }
  }
}

void WifiStatusJson::setConnectedAccessPoint(WifiAccessPoint *ap, String psk) {
  StringArray *cmd;
  
  // disconnect if no ap provided
  if (ap == nullptr) {
    connected = false;
    connectedAP = nullptr;
    for(const auto& listener : listeners) {
      listener->handleWifiDisconnected();
    }
  }
  // try to connect to ap, dispatch events on success and failure
  else {
    // FIXME: only until we get reading success over stdout hooked up
    bool isTestCred = ap->ssid == "NTC";
    if (!isTestCred) {
      DBG("WifiStatusJson::setConnectedAccessPoint - failed ");
      connected = false;
      connectedAP = nullptr;
      for(const auto& listener : listeners) {
        listener->handleWifiFailedConnect();
      }
      return;
    }
    
    if (psk.isEmpty()) {
      connected = true;
      connectedAP = ap;
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
    else {
      connected = true;
      connectedAP = ap;
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
  }
}

void WifiStatusJson::setDisconnected() {
  setConnectedAccessPoint(nullptr);
}

void WifiStatusJson::initializeStatus() {
  auto json = JSON::parse(assetFile("wifi.json"));
  connectedAP = nullptr;
  connected = false;

  accessPoints.clear();

  for (const auto &apJson : *json.getArray()) {
    auto ap = new WifiAccessPoint();
    ap->ssid = apJson["name"];
    ap->signalStrength = apJson["strength"];
    ap->requiresAuth = apJson["auth"];
    accessPoints.add(ap);
  }
}
