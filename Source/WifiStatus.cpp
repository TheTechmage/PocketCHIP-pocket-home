#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

WifiStatus::WifiStatus() : listeners() {}
WifiStatus::~WifiStatus() {}

void WifiStatus::addListener(Listener* listener) {
  listeners.add(listener);
}

// TODO: direct action should not be named set, e.g. enable/disable/disconnect
// otherwise easily confused with setters thats wrap members, which are slightly different idiom
void WifiStatus::setEnabled() {
  if (!enabled) {
    enabled = true;
    auto cmd = "nmcli radio wifi on";
    DBG("wifi cmd: " << cmd);
    ChildProcess().start(cmd);
    for(const auto& listener : listeners) {
      listener->handleWifiEnabled();
    }
  }
}

void WifiStatus::setDisabled() {
  if (enabled) {
    enabled = false;
    auto cmd = "nmcli radio wifi off";
    DBG("wifi cmd: " << cmd);
    ChildProcess().start(cmd);
    for(const auto& listener : listeners) {
      listener->handleWifiDisabled();
    }
  }
}

void WifiStatus::setConnectedAccessPoint(WifiAccessPoint *ap, String psk) {
  char* cmd;
  
  // disconnect if no ap provided
  if (ap == nullptr) {
    asprintf(&cmd, "nmcli dev disconnect iface wlan0");
    connected = false;
    connectedAccessPoint = nullptr;
    for(const auto& listener : listeners) {
      listener->handleWifiDisconnected();
    }
  }
  // try to connect to ap, dispatch events on success and failure
  else {
    // FIXME: only until we get reading success over stdout hooked up
    bool isTestCred = ap->ssid == "NTC 2461";
    if (!isTestCred) {
      DBG("WifiStatus::setConnectedAccessPoint - failed ");
      for(const auto& listener : listeners) {
        listener->handleWifiFailedConnect();
      }
      return;
    }
    
    if (!ap->requiresAuth) {
      asprintf(&cmd, "nmcli dev wifi con \"%s\"", ap->ssid.toRawUTF8());
      connected = true;
      connectedAccessPoint = ap;
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
    else {
      connected = true;
      connectedAccessPoint = ap;
      asprintf(&cmd, "nmcli dev wifi con \"%s\" password \"%s\"", ap->ssid.toRawUTF8(), psk.toRawUTF8());
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
  }
  
  if (cmd) {
    DBG("WifiStatus cmd: " << cmd);
    ChildProcess().start(cmd);
  }
}

void WifiStatus::setDisconnected() {
  setConnectedAccessPoint(nullptr);
}

void WifiStatus::populateFromJson(const var &json) {
  connectedAccessPoint = nullptr;
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

WifiStatus::Listener::Listener() {}
WifiStatus::Listener::~Listener() {}

