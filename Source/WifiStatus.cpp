#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

// TODO: direct action should not be named set, e.g. enable/disable/disconnect
// otherwise easily confused with setters that wrap members, which are slightly different idiom
void WifiStatus::setEnabled() {
  if (!enabled) {
    enabled = true;
    auto cmd = "nmcli radio wifi on";
    DBG("wifi cmd: " << cmd);
    ChildProcess().start(cmd);
  }
}

void WifiStatus::setDisabled() {
  if (enabled) {
    enabled = false;
    auto cmd = "nmcli radio wifi off";
    DBG("wifi cmd: " << cmd);
    ChildProcess().start(cmd);
  }
}

void WifiStatus::setConnectedAccessPoint(WifiAccessPoint *ap, String psk) {
  connected = ap != nullptr;
  connectedAccessPoint = ap;
  
  char* cmd;
  if (ap == nullptr) {
    asprintf(&cmd, "nmcli dev disconnect iface wlan0");
  }
  else if (psk.isEmpty()) {
    asprintf(&cmd, "nmcli dev wifi con \"%s\"", ap->ssid.toRawUTF8());
  }
  else {
    asprintf(&cmd, "nmcli dev wifi con \"%s\" password \"%s\"", ap->ssid.toRawUTF8(), psk.toRawUTF8());
  }
  DBG("wifi cmd: " << cmd);
  ChildProcess().start(cmd);
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

