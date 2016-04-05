#include "WifiStatus.h"

void WifiStatus::setConnectedAccessPoint(WifiAccessPoint *ap) {
  connected = ap != nullptr;
  connectedAccessPoint = ap;
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

