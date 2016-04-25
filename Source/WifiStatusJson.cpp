#include <map>

#include "Utils.h"

#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

WifiStatusJson::WifiStatusJson() : listeners() {}
WifiStatusJson::~WifiStatusJson() {}

OwnedArray<WifiAccessPoint> WifiStatusJson::nearbyAccessPoints() {
  OwnedArray<WifiAccessPoint> accessPoints;
  auto json = JSON::parse(assetFile("wifi.json"));

  for (const auto &apJson : *json.getArray()) {
    auto ap = new WifiAccessPoint();
    ap->ssid = apJson["name"];
    ap->signalStrength = apJson["strength"];
    ap->requiresAuth = apJson["auth"];
    ap->hash = apJson["name"];
    accessPoints.add(ap);
  }
  return accessPoints;
}

WifiAccessPoint WifiStatusJson::connectedAccessPoint() const {
  return WifiAccessPoint(*connectedAP);
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

void WifiStatusJson::clearListeners() {
  listeners.clear();
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
  // disconnect if no ap provided
  if (ap == nullptr) {
    DBG("WifiStatusJson::setConnectedAccessPoint - disconnect");
    connected = false;
    connectedAP = nullptr;
    for(const auto& listener : listeners) {
      listener->handleWifiDisconnected();
    }
  }
  // try to connect to ap, dispatch events on success and failure
  else {
    bool isTestCred = ap->ssid == "MyFi";
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
      DBG("WifiStatusJson::setConnectedAccessPoint - connect with psk");
      connected = true;
      connectedAP = ap;
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
    else {
      DBG("WifiStatusJson::setConnectedAccessPoint - connect");
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
  connectedAP = nullptr;
  connected = false;
}
