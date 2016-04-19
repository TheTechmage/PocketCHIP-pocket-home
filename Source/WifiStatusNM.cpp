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

  if (!nmdevice || !NM_IS_DEVICE(nmdevice))
    nmdevice = nm_client_get_device_by_iface(nmclient, "wlan0");

  if (!nmdevice || !NM_IS_DEVICE(nmdevice))
    DBG("WifiStatusNM: failed to connect to nmdevice wlan0 over dbus");

  return nmclient;
}

bool resolveAPSecurity(NMAccessPoint *ap) {
  //FIXME: Assumes all security types equal
  return (
    nm_access_point_get_flags(ap) == NM_802_11_AP_FLAGS_PRIVACY ||
    nm_access_point_get_wpa_flags(ap) != NM_802_11_AP_SEC_NONE ||
    nm_access_point_get_rsn_flags(ap) != NM_802_11_AP_SEC_NONE
  );
}

WifiAccessPoint *createWifiAccessPoint(NMAccessPoint *ap) {
  GBytes *ssid = nm_access_point_get_ssid(ap);
  char *ssid_str = NULL, *ssid_hex_str = NULL;
  bool security = resolveAPSecurity(ap);

  /* Convert to strings */
  if (ssid) {
    const guint8 *ssid_data;
    gsize ssid_len;

    ssid_data = (const guint8 *) g_bytes_get_data(ssid, &ssid_len);
    ssid_str = nm_utils_ssid_to_utf8(ssid_data, ssid_len);
  }

  if (!ssid_str || !ssid) {
    DBG("libnm conversion of ssid to utf8 failed, skipping");
    return NULL;
  }

  return new WifiAccessPoint {
    ssid_str,
    1, //nm_access_point_get_strength(ap),
    security,
  };
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

WifiAccessPoint* getNMConnectedAP(NMDeviceWifi *wdev) {
  NMAccessPoint *ap = nm_device_wifi_get_active_access_point(wdev);

  if (!wdev || !ap)
    return NULL;

  return createWifiAccessPoint(ap);
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

static void handle_wireless_connected(WifiStatusNM *wifiStatus) {
  DBG("SIGNAL: " << NM_DEVICE_STATE << ": changed! ");
  wifiStatus->handleWirelessConnected();
}

static void handle_active_access_point(WifiStatusNM *wifiStatus) {
  DBG("SIGNAL: " << NM_DEVICE_WIFI_ACTIVE_ACCESS_POINT << ": changed! ");
  wifiStatus->handleConnectedAccessPoint();
}

static void handle_add_and_activate_finish(GObject *client,
                                           GAsyncResult *result,
                                           gpointer user_data) {
  NMActiveConnection *active;
  GError *err = NULL;

  active = nm_client_add_and_activate_connection_finish(NM_CLIENT(client), result, &err);
  if (err) {
    DBG("WifiStatusNM: failed to add/activate connection!");
    DBG("WifiStatusNM::" << __func__ << ": " << err->message);
    g_error_free(err);
  }
}


void NMListener::initialize(WifiStatusNM *status, NMClient *client) {
  nm = client;
  wifiStatus = status;
}

void NMListener::run() {
  NMDeviceWifi *wdev = NM_DEVICE_WIFI(nm_client_get_device_by_iface(nm, "wlan0"));
  context = g_main_context_default();
  //context = g_main_context_new();
  loop = g_main_loop_new(context, false);
  //g_main_context_invoke(context, initialize_in_context, status);

  g_signal_connect_swapped(nm, "notify::" NM_CLIENT_WIRELESS_ENABLED,
    G_CALLBACK(handle_wireless_enabled), wifiStatus);

  g_signal_connect_swapped(NM_DEVICE(wdev), "notify::" NM_DEVICE_STATE,
    G_CALLBACK(handle_wireless_connected), wifiStatus);

  g_signal_connect_swapped(wdev, "notify::" NM_DEVICE_WIFI_ACTIVE_ACCESS_POINT,
    G_CALLBACK(handle_active_access_point), wifiStatus);

  while (!threadShouldExit()) {
    {
      const MessageManagerLock mmLock;
      bool dispatched = g_main_context_iteration(context, false);
    }
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

void WifiStatusNM::handleWirelessConnected() {
  NMDeviceState state = nm_device_get_state(nmdevice);
  DBG("WifiStatusNM::" << __func__ << " changed to " << state
      << " while connecting = " << connecting);

  switch (state) {
    case NM_DEVICE_STATE_ACTIVATED:
      if (connected)
        break;

      connected = true;
      connecting = false;
      DBG("WifiStatus::" << __func__ << " - connected");
      for(const auto& listener : listeners)
        listener->handleWifiConnected();
      break;

    case NM_DEVICE_STATE_PREPARE:
    case NM_DEVICE_STATE_CONFIG:
    case NM_DEVICE_STATE_IP_CONFIG:
    case NM_DEVICE_STATE_IP_CHECK:
    case NM_DEVICE_STATE_SECONDARIES:
      /* No state change for now, wait for connection to complete/fail */
      break;

    case NM_DEVICE_STATE_NEED_AUTH:
      /* FIXME: let this drop into the general failed case for now
       *        eventually this should prompt the user
       */
    case NM_DEVICE_STATE_DISCONNECTED:
    case NM_DEVICE_STATE_DEACTIVATING:
    case NM_DEVICE_STATE_FAILED:
      if (!connected)
        break;
      
      connected = false;

      if (connecting)
        connecting = false;
        DBG("WifiStatus::" << __func__ << " - failed");
        for(const auto& listener : listeners)
          listener->handleWifiFailedConnect();
        break;

      for(const auto& listener : listeners)
        listener->handleWifiDisconnected();
      break;

    case NM_DEVICE_STATE_UNKNOWN:
    case NM_DEVICE_STATE_UNMANAGED:
    case NM_DEVICE_STATE_UNAVAILABLE:
    default:
      if (connecting || connected) {
        std::cerr << "WifiStatusNM::" << __func__
                  << ": wlan0 device entered unmanaged state: " << state << std::endl;
        connected = false;
        connecting = false;
        for(const auto& listener : listeners)
          listener->handleWifiDisconnected();
      }
  }
}

void WifiStatusNM::handleConnectedAccessPoint() {
  DBG("WifiStatusNM::" << __func__ << " changed active AP");
  auto connectedAP = getNMConnectedAP(NM_DEVICE_WIFI(nmdevice));
}

void WifiStatusNM::setConnectedAccessPoint(WifiAccessPoint *ap, String psk) {
  ScopedPointer<StringArray> cmd;
  
  // disconnect if no ap provided
  if (ap == nullptr) {
    NMActiveConnection *conn = nm_device_get_active_connection(nmdevice);

    if (conn) {
      GError *err = NULL;
      NMRemoteConnection *rconn = nm_active_connection_get_connection(conn);
      nm_remote_connection_delete(rconn, NULL, &err);
      if (err) {
	DBG("WifiStatusNM: failed to remove active connection!");
	DBG("WifiStatusNM::" << __func__ << ": " << err->message);
	g_error_free(err);
      }
    }
    return;
  }
  // try to connect to ap, dispatch events on success and failure
  else {
    NMConnection *connection = NULL;
    NMSettingWireless *s_wifi = NULL;
    NMSettingWirelessSecurity *s_wsec = NULL;
    const char *nm_ap_path = NULL;
    const GPtrArray *ap_list;
    GBytes *candidate_ssid;
    NMAccessPoint *candidate_ap;

    //FIXME: expand WifiAccessPoint struct to know which NMAccessPoint it is
    ap_list = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(nmdevice));
    for (int i = 0; i < ap_list->len; i++) {
      char *ssid;
      candidate_ap = (NMAccessPoint *) g_ptr_array_index(ap_list, i);

      candidate_ssid = nm_access_point_get_ssid(candidate_ap);
      if (!candidate_ssid)
        break;

      ssid = nm_utils_ssid_to_utf8((const guint8 *) g_bytes_get_data(candidate_ssid, NULL),
                                                  g_bytes_get_size(candidate_ssid));

      if (ssid && ap->ssid == ssid) {
        nm_ap_path = nm_object_get_path(NM_OBJECT(candidate_ap));
        g_free(ssid);
        break;
      }
    }

    if (!nm_ap_path)
      return;

    connecting = true;

    connection = nm_simple_connection_new();
    s_wifi = (NMSettingWireless *) nm_setting_wireless_new();
    nm_connection_add_setting(connection, NM_SETTING(s_wifi));
    g_object_set(s_wifi,
                 NM_SETTING_WIRELESS_SSID, candidate_ssid,
                 NM_SETTING_WIRELESS_HIDDEN, false,
                 NULL);

    if (!psk.isEmpty()) {
      s_wsec = (NMSettingWirelessSecurity *) nm_setting_wireless_security_new();
      nm_connection_add_setting(connection, NM_SETTING(s_wsec));
      g_object_set(s_wsec, NM_SETTING_WIRELESS_SECURITY_PSK, psk.toRawUTF8(), NULL);
      /* FIXME: Assuming WPA/WPA2 PSK ... skipping WEP and WPA-Enterprise
      nm_setting_wireless_security_set_wep_key(s_wsec, 0, password);
      g_object_set(G_OBJECT(s_wsec),
                   NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE,
                   wep_passphrase ? NM_WEP_KEY_TYPE_PASSPHRASE: NM_WEP_KEY_TYPE_KEY,
                   NULL);
      */
    }

    nm_client_add_and_activate_connection_async(nmclient,
                                                connection,
                                                nmdevice,
                                                nm_ap_path,
                                                NULL,
                                                handle_add_and_activate_finish,
                                                NULL);
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

  connected = nm_device_get_state(nmdevice) == NM_DEVICE_STATE_ACTIVATED;

  if (connected)
    connectedAP = getNMConnectedAP(NM_DEVICE_WIFI(nmdevice));

  accessPoints.clear();

  getNMAvailableAccessPoints(accessPoints);
}
