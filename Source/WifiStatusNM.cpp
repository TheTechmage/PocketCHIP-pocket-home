#include <map>

#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

// From libnm-util/NetworkManager.h
typedef enum {
        NM_DEVICE_STATE_UNKNOWN      = 0,
        NM_DEVICE_STATE_UNMANAGED    = 10,
        NM_DEVICE_STATE_UNAVAILABLE  = 20,
        NM_DEVICE_STATE_DISCONNECTED = 30,
        NM_DEVICE_STATE_PREPARE      = 40,
        NM_DEVICE_STATE_CONFIG       = 50,
        NM_DEVICE_STATE_NEED_AUTH    = 60,
        NM_DEVICE_STATE_IP_CONFIG    = 70,
        NM_DEVICE_STATE_IP_CHECK     = 80,
        NM_DEVICE_STATE_SECONDARIES  = 90,
        NM_DEVICE_STATE_ACTIVATED    = 100,
        NM_DEVICE_STATE_DEACTIVATING = 110,
        NM_DEVICE_STATE_FAILED       = 120
} NMDeviceState;


WifiStatusNM::WifiStatusNM() : listeners() {}
WifiStatusNM::~WifiStatusNM() {}

std::vector<String> split(const String &orig, const String &delim) {
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

bool isNMWifiRadioEnabled() {
  ChildProcess nmproc;
  String state;
  auto cmd = new StringArray({"nmcli","r","wifi"});

  DBG("WifiStatusNM cmd: " << cmd->joinIntoString(" "));
  nmproc.start(*cmd);
  nmproc.waitForProcessToFinish(1000);
  state = nmproc.readAllProcessOutput();
  DBG("Output: \n" << state);

  if (state.trim() == "enabled")
    return true;
  else
    return false;
}

bool getNMWifiConnectionProperty(const String propName, const String connName, String &val) {
  ChildProcess nmproc;
  String propertyOutput;
  auto cmd = new StringArray({"nmcli","-f",propName.toRawUTF8(),"c","show","id",connName.toRawUTF8()});

  DBG("WifiStatusNM cmd: " << cmd->joinIntoString(" "));
  nmproc.start(*cmd);
  nmproc.waitForProcessToFinish(1000);
  propertyOutput = nmproc.readAllProcessOutput();
  DBG("Output: \n" << propertyOutput);

  auto key_val = split(propertyOutput, ":");
  if (key_val[0] == propName) {
    val = key_val[1].trim();
    return true;
  } else {
    return false;
  }
}

bool getNMWifiDeviceProperty(const String propName, String &val) {
  ChildProcess nmproc;
  String propertyOutput;
  auto cmd = new StringArray({"nmcli","-f",propName.toRawUTF8(),"d","show","wlan0"});

  DBG("WifiStatusNM cmd: " << cmd->joinIntoString(" "));
  nmproc.start(*cmd);
  nmproc.waitForProcessToFinish(1000);
  propertyOutput = nmproc.readAllProcessOutput();
  DBG("Output: \n" << propertyOutput);

  auto key_val = split(propertyOutput, ":");
  if (key_val[0] == propName) {
    val = key_val[1].trim();
    return true;
  } else {
    return false;
  }
}

int getNMWifiConnectionState() {
  String stateString;

  if (getNMWifiDeviceProperty("GENERAL.STATE", stateString)) {
    auto state = split(stateString.trim()," ")[0];
    return state.getIntValue();
  } else {
    return NM_DEVICE_STATE_UNKNOWN;
  }
}

bool isNMWifiConnected() {
  switch (getNMWifiConnectionState()) {
    case NM_DEVICE_STATE_UNKNOWN:
    case NM_DEVICE_STATE_UNMANAGED:
    case NM_DEVICE_STATE_UNAVAILABLE:
    case NM_DEVICE_STATE_DISCONNECTED:
      return false;
    case NM_DEVICE_STATE_PREPARE:
    case NM_DEVICE_STATE_CONFIG:
    case NM_DEVICE_STATE_NEED_AUTH:
    case NM_DEVICE_STATE_IP_CONFIG:
    case NM_DEVICE_STATE_IP_CHECK:
    case NM_DEVICE_STATE_SECONDARIES:
      return false;
    case NM_DEVICE_STATE_ACTIVATED:
    case NM_DEVICE_STATE_DEACTIVATING:
    case NM_DEVICE_STATE_FAILED:
      return true;
  }
}

String getNMWifiConnectionName() {
  String name;

  if (getNMWifiDeviceProperty("GENERAL.CONNECTION", name)) {
    return name;
  } else {
    return "";
  }
}

String getNMWifiConnectedSSID() {
  String connName = getNMWifiConnectionName();
  String ssid;

  if (getNMWifiConnectionProperty("802-11-wireless.ssid", connName, ssid)) {
    return ssid;
  } else {
    return "";
  }
}

void getNMConnectedAP(WifiAccessPoint *ap) {
  ap = new WifiAccessPoint {
      getNMWifiConnectedSSID(),
      1, //FIXME: Clearly wrong assumption
      false, //FIXME: Clearly wrong assumption
    };
}

void getNMAvailableAccessPoints(OwnedArray<WifiAccessPoint> &aps) {
  String ssidList;
  std::map<String, String> tag_map;
  ChildProcess nmproc;

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

  for (const String& tag : split(ssidList, "\n")) {
    auto key_val = split(tag, ":");
    if (key_val[0] == "SSID" && !tag_map.empty()) {
      DBG("Adding non-empty tagmap to accessPoints");
      addAccessPoint(tag_map, aps);
      tag_map.clear();
    }
    tag_map.insert(std::make_pair(key_val[0], key_val[1].trimStart()));
  }
  addAccessPoint(tag_map, aps);
  tag_map.clear();
}

OwnedArray<WifiAccessPoint> *WifiStatusNM::nearbyAccessPoints() {
  getNMAvailableAccessPoints(accessPoints);
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
    /* FIXME Without launching scans, the results of a disable/enable are confusing
     * so ignore the enable/disable events for now
    */
    auto cmd = "nmcli radio wifi on";
    DBG("wifi cmd: " << cmd);
    ChildProcess nmproc;
    nmproc.start(cmd);
    nmproc.waitForProcessToFinish(500);
    enabled = isNMWifiRadioEnabled();
    for(const auto& listener : listeners) {
      listener->handleWifiEnabled();
    }
  }
}

void WifiStatusNM::setDisabled() {
  if (enabled) {
    /* FIXME Without launching scans, the results of a disable/enable are confusing
     * so ignore the enable/disable events for now
    */
    auto cmd = "nmcli radio wifi off";
    DBG("wifi cmd: " << cmd);
    ChildProcess nmproc;
    nmproc.start(cmd);
    nmproc.waitForProcessToFinish(500);
    enabled = isNMWifiRadioEnabled();
    for(const auto& listener : listeners) {
      listener->handleWifiDisabled();
    }
  }
}

void WifiStatusNM::setConnectedAccessPoint(WifiAccessPoint *ap, String psk) {
  ScopedPointer<StringArray> cmd;
  
  // disconnect if no ap provided
  if (ap == nullptr) {
    ChildProcess nmproc;
    String profileList;
    String profileName;
    bool name_on_next_line = false;

    cmd = new StringArray({"nmcli","-m","multiline","-f","DEVICE,NAME","c","show","--active"});

    DBG("WifiStatusNM cmd: " << cmd->joinIntoString(" "));
    nmproc.start(*cmd);
    nmproc.waitForProcessToFinish(1000);
    profileList = nmproc.readAllProcessOutput();
    DBG("Output: \n" << profileList);

    for (const String& tag : split(profileList, "\n")) {
      auto key_val = split(tag, ":");
      if (name_on_next_line) {
        profileName = key_val[1].trimStart();
	DBG("Found profile name: " << profileName);
      }

      if (key_val[0] == "DEVICE" && key_val[1].trimStart() == "wlan0")
        name_on_next_line = true;
      else
	name_on_next_line = false;
    }

    cmd = new StringArray({"nmcli","c","delete","id",profileName.toRawUTF8()});
    connected = false;
    connectedAP = nullptr;
    DBG("WifiStatusNM cmd: " << cmd->joinIntoString(" "));
    nmproc.start(*cmd);
    nmproc.waitForProcessToFinish(1000);
    DBG("Output: \n" << profileList);
    for(const auto& listener : listeners) {
      listener->handleWifiDisconnected();
    }
    return;
  }
  // try to connect to ap, dispatch events on success and failure
  else {
    if (psk.isEmpty()) {
      cmd = new StringArray({"nmcli","dev","wifi","connect",ap->ssid.toRawUTF8(),"iface","wlan0"});
    }
    else {
      cmd = new StringArray({"nmcli","dev","wifi","connect",ap->ssid.toRawUTF8(),"password",psk.toRawUTF8(),"iface","wlan0"});
    }
  }
  
  if (cmd) {
    DBG("WifiStatusNM cmd: " << cmd->joinIntoString(" "));
    ChildProcess nmproc;
    nmproc.start(*cmd);
    nmproc.waitForProcessToFinish(30000);

    auto exitCode = nmproc.getExitCode();
    DBG("WifiStatus exitCode: " << String(exitCode));
    bool success = exitCode == 0;
    if (success) {
      connected = true;
      getNMConnectedAP(connectedAP);
      DBG("WifiStatus::setConnectedAccessPoint - success");
      for(const auto& listener : listeners) {
        listener->handleWifiConnected();
      }
    }
    else {
      DBG("WifiStatus::setConnectedAccessPoint - failed");
      for(const auto& listener : listeners) {
        listener->handleWifiFailedConnect();
      }
    }
  }
}

void WifiStatusNM::setDisconnected() {
  setConnectedAccessPoint(nullptr);
}

void WifiStatusNM::initializeStatus() {
  connectedAP = nullptr;
  connected = false;

  enabled = isNMWifiRadioEnabled();

  if (!enabled)
    return;

  connected = isNMWifiConnected();

  if (connected)
    getNMConnectedAP(connectedAP);

  accessPoints.clear();

  getNMAvailableAccessPoints(accessPoints);
}
