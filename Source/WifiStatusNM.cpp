#include <map>

#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

WifiStatusNM::WifiStatusNM() : listeners() {}
WifiStatusNM::~WifiStatusNM() {}

OwnedArray<WifiAccessPoint> *WifiStatusNM::nearbyAccessPoints() {
  return &accessPoints;
}

WifiAccessPoint *WifiStatusNM::connectedAccessPoint() const {
  return connectedAP;
}

bool WifiStatusNM::isEnabled() const {
  return enabled;
}

bool WifiStatusNM::isConnected() const {
  return connected;
}

void WifiStatusNM::addListener(Listener* listener) {
  listeners.add(listener);
}

// TODO: direct action should not be named set, e.g. enable/disable/disconnect
// otherwise easily confused with setters thats wrap members, which are slightly different idiom
void WifiStatusNM::setEnabled() {
  if (!enabled) {
    enabled = true;
    /* FIXME Without launching scans, the results of a disable/enable are confusing
     * so ignore the enable/disable events for now
    auto cmd = "nmcli radio wifi on";
    DBG("wifi cmd: " << cmd);
    ChildProcess nmproc;
    nmproc.start(cmd);
    nmproc.waitForProcessToFinish(500);
    */
    for(const auto& listener : listeners) {
      listener->handleWifiEnabled();
    }
  }
}

void WifiStatusNM::setDisabled() {
  if (enabled) {
    enabled = false;
    /* FIXME Without launching scans, the results of a disable/enable are confusing
     * so ignore the enable/disable events for now
    auto cmd = "nmcli radio wifi off";
    DBG("wifi cmd: " << cmd);
    ChildProcess nmproc;
    nmproc.start(cmd);
    nmproc.waitForProcessToFinish(500);
    */
    for(const auto& listener : listeners) {
      listener->handleWifiDisabled();
    }
  }
}

void WifiStatusNM::setConnectedAccessPoint(WifiAccessPoint *ap, String psk) {
  StringArray *cmd;
  
  // disconnect if no ap provided
  if (ap == nullptr) {
    // FIXME: doesn't work because it requires root. It puts NM into 'manual' mode
    // so reconnection doesn't occur until reboot or explicit reconnection.
    cmd = new StringArray({"nmcli","dev","disconnect","wlan0"});
    connected = false;
    connectedAP = nullptr;
    for(const auto& listener : listeners) {
      listener->handleWifiDisconnected();
    }
  }
  // try to connect to ap, dispatch events on success and failure
  else {
    // FIXME: only until we get reading success over stdout hooked up
    bool isTestCred = ap->ssid == "NTC 2461";
    if (!isTestCred) {
      DBG("WifiStatusNM::setConnectedAccessPoint - failed ");
      for(const auto& listener : listeners) {
        listener->handleWifiFailedConnect();
      }
      return;
    }
    
    if (psk.isEmpty()) {
      cmd = new StringArray({"nmcli","dev","wifi","connect",ap->ssid.toRawUTF8(),"iface","wlan0"});
      connected = true;
      connectedAP = ap;
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
    else {
      cmd = new StringArray({"nmcli","dev","wifi","connect",ap->ssid.toRawUTF8(),"password",psk.toRawUTF8(),"iface","wlan0"});
      connected = true;
      connectedAP = ap;
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
  }
  
  if (cmd) {
    DBG("WifiStatusNM cmd: " << cmd->joinIntoString(" "));
    ChildProcess nmproc;
    nmproc.start(*cmd);
    nmproc.waitForProcessToFinish(30000);
    // TODO: add result reading here to send correct callbacks to UI
  }
}

void WifiStatusNM::setDisconnected() {
  setConnectedAccessPoint(nullptr);
}

void WifiStatusNM::initializeStatus() {
  connectedAP = nullptr;
  connected = false;
  String ssidList;
  std::map<String, String> tag_map;
  ChildProcess nmproc;

  accessPoints.clear();

  auto cmd = "nmcli -m multiline -f SSID,SECURITY,SIGNAL d wifi list ifname wlan0";
  DBG("WifiStatusNM cmd: " << cmd);
  nmproc.start(cmd);
  nmproc.waitForProcessToFinish(500);
  ssidList = nmproc.readAllProcessOutput();

  auto addAccessPoint = [](std::map<String, String> &keyvals, OwnedArray<WifiAccessPoint> &aps) {
    aps.add( new WifiAccessPoint {
      keyvals["SSID"] == "--" ? "HiddenSSID" : keyvals["SSID"],
      1, // keyvals["SIGNAL"].getIntValue()
      keyvals["SECURITY"].isNotEmpty(), //FIXME: Assumes all security types equal
    });
  };

  auto split = [](const String &orig, const String &delim) {
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
