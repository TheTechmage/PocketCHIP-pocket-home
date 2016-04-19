#include <map>

#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

WifiStatusNM::WifiStatusNM() : listeners() {}
WifiStatusNM::~WifiStatusNM() {}

NMClient* WifiStatusNM::connectToNetworkManager() {
  if (!nmclient || !NM_IS_CLIENT(nmclient))
    nmclient = nm_client_new(NULL, NULL);

  if (!nmclient || !NM_IS_CLIENT(nmclient))
    DBG("WifiStatusNM: failed to connect to nmclient over dbus");

  return nmclient;
}

bool isNMWifiRadioEnabled() {
  ChildProcess nmproc;
  StringArray cmd{"nmcli","r","wifi"};
  String state;

  DBG("WifiStatusNM cmd: " << cmd.joinIntoString(" "));
  nmproc.start(cmd);
  nmproc.waitForProcessToFinish(1000);
  state = nmproc.readAllProcessOutput();

  if (state.trim() == "enabled")
    return true;
  else
    return false;
}

bool getNMWifiConnectionProperty(const String propName, const String connName, String &val) {
  ChildProcess nmproc;
  String propertyOutput;
  StringArray cmd{"nmcli","-f",propName.toRawUTF8(),"c","show","id",connName.toRawUTF8()};

  DBG("WifiStatusNM cmd: " << cmd.joinIntoString(" "));
  nmproc.start(cmd);
  nmproc.waitForProcessToFinish(1000);
  propertyOutput = nmproc.readAllProcessOutput();

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
  StringArray cmd{"nmcli","-f",propName.toRawUTF8(),"d","show","wlan0"};

  DBG("WifiStatusNM cmd: " << cmd.joinIntoString(" "));
  nmproc.start(cmd);
  nmproc.waitForProcessToFinish(1000);
  propertyOutput = nmproc.readAllProcessOutput();

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
    default:
      return false;
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

WifiAccessPoint* getNMConnectedAP() {
  return new WifiAccessPoint {
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

NMListener::NMListener() : Thread("NMListener Thread") {}

NMListener::~NMListener() {
  DBG(__func__ << ": cleanup thread");
  if (isThreadRunning()) {
    signalThreadShouldExit();
    notify();
    stopThread(2000);
  }
}

static void handle_wireless_enabled(WifiStatusNM *wifiStatus) {
  DBG("SIGNAL: " << NM_CLIENT_WIRELESS_ENABLED << ": changed! ");
  wifiStatus->handleWirelessEnabled();
}

void NMListener::initialize(WifiStatusNM *status, NMClient *client) {
  nm = client;
  wifiStatus = status;
}

void NMListener::run() {
  context = g_main_context_default();
  //context = g_main_context_new();
  loop = g_main_loop_new(context, false);
  //g_main_context_invoke(context, initialize_in_context, status);

  g_signal_connect_swapped(nm, "notify::" NM_CLIENT_WIRELESS_ENABLED,
    G_CALLBACK(handle_wireless_enabled), wifiStatus);

  while (!threadShouldExit()) {
    bool dispatched = g_main_context_iteration(context, false);
    wait(100);
  }

  g_main_loop_unref(loop);
  g_main_context_unref(context);
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
  if (!enabled)
    nm_client_wireless_set_enabled(nmclient, true);
}

void WifiStatusNM::setDisabled() {
  if (enabled)
    nm_client_wireless_set_enabled(nmclient, false);
}

void WifiStatusNM::handleWirelessEnabled() {
  enabled = nm_client_wireless_get_enabled(nmclient);
  DBG("WifiStatusNM::" << __func__ << " changed to " << enabled);

  //FIXME: Force and wait for a scan after enable
  if (enabled)
    for (const auto& listener : listeners)
      listener->handleWifiEnabled();
  else
    for (const auto& listener : listeners)
      listener->handleWifiDisabled();

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
      connectedAP = getNMConnectedAP();
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

  if (!this->connectToNetworkManager())
    DBG("WifiStatusNM: failed to connect to nmclient over dbus");

  nmlistener = new NMListener();
  nmlistener->initialize(this, nmclient);
  nmlistener->startThread();

  enabled = nm_client_wireless_get_enabled(nmclient);

  if (!enabled)
    return;

  connected = isNMWifiConnected();

  if (connected)
    connectedAP = getNMConnectedAP();

  accessPoints.clear();

  getNMAvailableAccessPoints(accessPoints);
}
