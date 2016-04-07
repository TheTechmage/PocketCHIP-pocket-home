#include <map>

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
  connected = ap != nullptr;
  connectedAccessPoint = ap;
  
  char* cmd;
  if (ap == nullptr) {
    asprintf(&cmd, "nmcli dev disconnect iface wlan0");
    for(const auto& listener : listeners) {
      listener->handleWifiDisconnected();
    }
  }
  else if (psk.isEmpty()) {
    asprintf(&cmd, "nmcli dev wifi con \"%s\"", ap->ssid.toRawUTF8());
    for(const auto& listener : listeners) {
      listener->handleWifiConnected();
    }
  }
  else {
    asprintf(&cmd, "nmcli dev wifi con \"%s\" password \"%s\"", ap->ssid.toRawUTF8(), psk.toRawUTF8());
    for(const auto& listener : listeners) {
      listener->handleWifiConnected();
    }
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
  String ssidList;
  std::map<String, String> tag_map;
  ChildProcess nmproc;

  accessPoints.clear();

  auto cmd = "nmcli -m multiline -f SSID,SECURITY,SIGNAL d wifi list ifname wlan0";
  DBG("wifi cmd: " << cmd);
  nmproc.start(cmd);
  ssidList = nmproc.readAllProcessOutput();

  auto addAccessPoint = [](std::map<String, String> &keyvals, OwnedArray<WifiAccessPoint> &aps) {
    aps.add( new WifiAccessPoint {
      keyvals["SSID"] == "--" ? "HiddenSSID" : keyvals["SSID"],
      keyvals["SIGNAL"].getIntValue(),
      keyvals["SECURITY"].isNotEmpty(), //FIXME: Assumes all security types equal
    });
  };

  auto split = [](const String &orig, const String delim) {
    std::vector<String> elems;
    int index = 0;
    auto remainder = orig.substring(index);
    while (remainder.length()) {
      index = remainder.indexOf(delim);
      if (index < 0) {
        elems.push_back(remainder);
	break;
      }
      elems.push_back(remainder.substring(0,index));
      remainder = remainder.substring(index+1);
    }
    return elems;
  };

  for (const String& tag : split(ssidList, "\n")) {
    auto key_val = split(tag, ":");
    DBG("Produced {" << key_val[0] << ": " << key_val[1].trimStart() << "}");
    if (key_val[0] == "SSID" && !tag_map.empty()) {
      DBG("Adding non-empty tagmap to accessPoints");
      addAccessPoint(tag_map, accessPoints);
      tag_map.clear();
    }
    tag_map.insert(std::make_pair(key_val[0], key_val[1].trimStart()));
  }
  addAccessPoint(tag_map, accessPoints);
  tag_map.clear();
}

WifiStatus::Listener::Listener() {}
WifiStatus::Listener::~Listener() {}

