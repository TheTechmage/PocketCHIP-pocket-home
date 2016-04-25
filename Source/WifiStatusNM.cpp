#ifdef LINUX

#include <nm-remote-connection.h>
#include <nm-utils.h>

#include "WifiStatus.h"
#include "../JuceLibraryCode/JuceHeader.h"

WifiStatusNM::WifiStatusNM() : listeners() {}
WifiStatusNM::~WifiStatusNM() {}

NMClient* WifiStatusNM::connectToNetworkManager() {
  if (!nmclient || !NM_IS_CLIENT(nmclient))
    nmclient = nm_client_new();

  if (!nmclient || !NM_IS_CLIENT(nmclient))
    DBG("WifiStatusNM: failed to connect to nmclient over dbus");

  if (!nmdevice || !NM_IS_DEVICE(nmdevice))
    nmdevice = nm_client_get_device_by_iface(nmclient, "wlan0");

  if (!nmdevice || !NM_IS_DEVICE(nmdevice))
    DBG("WifiStatusNM: failed to connect to nmdevice wlan0 over dbus");

  return nmclient;
}

/* Borrowed from network-manager-applet src/utils/utils.c */
char *
utils_hash_ap (const GByteArray *ssid,
               NM80211Mode mode,
               guint32 flags,
               guint32 wpa_flags,
               guint32 rsn_flags)
{
        unsigned char input[66];

        memset (&input[0], 0, sizeof (input));

        if (ssid)
                memcpy (input, ssid->data, ssid->len);

        if (mode == NM_802_11_MODE_INFRA)
                input[32] |= (1 << 0);
        else if (mode == NM_802_11_MODE_ADHOC)
                input[32] |= (1 << 1);
        else
                input[32] |= (1 << 2);

        /* Separate out no encryption, WEP-only, and WPA-capable */
        if (  !(flags & NM_802_11_AP_FLAGS_PRIVACY)
            && (wpa_flags == NM_802_11_AP_SEC_NONE)
            && (rsn_flags == NM_802_11_AP_SEC_NONE))
                input[32] |= (1 << 3);
        else if (   (flags & NM_802_11_AP_FLAGS_PRIVACY)
                 && (wpa_flags == NM_802_11_AP_SEC_NONE)
                 && (rsn_flags == NM_802_11_AP_SEC_NONE))
                input[32] |= (1 << 4);
        else if (   !(flags & NM_802_11_AP_FLAGS_PRIVACY)
                 &&  (wpa_flags != NM_802_11_AP_SEC_NONE)
                 &&  (rsn_flags != NM_802_11_AP_SEC_NONE))
                input[32] |= (1 << 5);
        else
                input[32] |= (1 << 6);

        /* duplicate it */
        memcpy (&input[33], &input[0], 32);
        return g_compute_checksum_for_data (G_CHECKSUM_MD5, input, sizeof (input));
}

bool resolveAPSecurity(NMAccessPoint *ap) {
  //FIXME: Assumes all security types equal
  return (
    nm_access_point_get_flags(ap) == NM_802_11_AP_FLAGS_PRIVACY ||
    nm_access_point_get_wpa_flags(ap) != NM_802_11_AP_SEC_NONE ||
    nm_access_point_get_rsn_flags(ap) != NM_802_11_AP_SEC_NONE
  );
}

WifiAccessPoint *createNMWifiAccessPoint(NMAccessPoint *ap) {
  const GByteArray *ssid = nm_access_point_get_ssid(ap);
  //GBytes *ssid = nm_access_point_get_ssid(ap);
  char *ssid_str = NULL, *ssid_hex_str = NULL;
  bool security = resolveAPSecurity(ap);

  /* Convert to strings */
  if (ssid) {
    const guint8 *ssid_data = ssid->data;
    gsize ssid_len = ssid->len;

    //ssid_data = (const guint8 *) g_bytes_get_data(ssid, &ssid_len);
    ssid_str = nm_utils_ssid_to_utf8(ssid);
  }

  if (!ssid_str || !ssid) {
    DBG("libnm conversion of ssid to utf8 failed, skipping");
    return NULL;
  }

  return new WifiAccessPoint {
    ssid_str,
    nm_access_point_get_strength(ap),
    security,
    utils_hash_ap(nm_access_point_get_ssid(ap),
                  nm_access_point_get_mode(ap),
                  nm_access_point_get_flags(ap),
                  nm_access_point_get_wpa_flags(ap),
                  nm_access_point_get_rsn_flags(ap)),
  };
}

void addNMWifiAccessPoints(gpointer data, gpointer user_data) {
  NMAccessPoint *ap = NM_ACCESS_POINT(data);
  OwnedArray<WifiAccessPoint> *aps = (OwnedArray<WifiAccessPoint> *) user_data;

  auto created_ap = createNMWifiAccessPoint(ap);
  if (created_ap)
    aps->add(created_ap);
}

WifiAccessPoint* getNMConnectedAP(NMDeviceWifi *wdev) {
  NMAccessPoint *ap = nm_device_wifi_get_active_access_point(wdev);

  if (!wdev || !ap) {
    DBG(__func__ << ": no NMAccessPoint found!");
    return NULL;
  }

  return createNMWifiAccessPoint(ap);
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

static void handle_add_and_activate_finish(NMClient *client,
                                           NMActiveConnection *active,
                                           const char* path,
                                           GError *err,
                                           gpointer user_data) {
  /*
  NMActiveConnection *active;
  GError *err = NULL;

  active = nm_client_add_and_activate_connection_finish(NM_CLIENT(client), result, &err);
  */
  if (err) {
    DBG("WifiStatusNM: failed to add/activate connection!");
    DBG("WifiStatusNM::" << __func__ << ": " << err->message);
    //g_error_free(err);
  }
}


void NMListener::initialize(WifiStatusNM *status, NMClient *client) {
  nm = client;
  wifiStatus = status;
}

void NMListener::run() {
  NMDevice *dev = nm_client_get_device_by_iface(nm, "wlan0");
  context = g_main_context_default();
  //context = g_main_context_new();
  loop = g_main_loop_new(context, false);
  //g_main_context_invoke(context, initialize_in_context, status);

  g_signal_connect_swapped(nm, "notify::" NM_CLIENT_WIRELESS_ENABLED,
    G_CALLBACK(handle_wireless_enabled), wifiStatus);

  g_signal_connect_swapped(dev, "notify::" NM_DEVICE_STATE,
    G_CALLBACK(handle_wireless_connected), wifiStatus);

  g_signal_connect_swapped(NM_DEVICE_WIFI(dev), "notify::" NM_DEVICE_WIFI_ACTIVE_ACCESS_POINT,
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

OwnedArray<WifiAccessPoint> WifiStatusNM::nearbyAccessPoints() {
  NMDeviceWifi *wdev;
  OwnedArray<WifiAccessPoint> accessPoints;

  wdev = NM_DEVICE_WIFI(nmdevice);
  //nm_device_wifi_request_scan(wdev, NULL, NULL);

  g_ptr_array_foreach(
      (GPtrArray *) nm_device_wifi_get_access_points(wdev),
      addNMWifiAccessPoints, &accessPoints);

  DBG(__func__ << ": found " << accessPoints.size() << " AccessPoints");
  return accessPoints;
}

WifiAccessPoint WifiStatusNM::connectedAccessPoint() const {
  const char * state = (connectedAP == nullptr) ? "NULL" : "ADDR";
  DBG(__func__ << ": connectedAP points to " << state);
  return WifiAccessPoint(*connectedAP);
}

bool WifiStatusNM::isEnabled() const {
  return enabled;
}

bool WifiStatusNM::isConnected() const {
  return connected;
}

void WifiStatusNM::addListener(Listener* listener) {
  listeners.add(listener);
  DBG("WifiStatusNM::" << __func__ << " numListeners = " << listeners.size());
}

void WifiStatusNM::clearListeners() {
  listeners.clear();
  DBG("WifiStatusNM::" << __func__ << " numListeners = " << listeners.size());
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

      handle_active_access_point(this);
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
      if (connecting) {
        connected = false;
        connecting = false;
        DBG("WifiStatus::" << __func__ << " - failed");
        for(const auto& listener : listeners)
          listener->handleWifiFailedConnect();
        break;
      }

      if (!connected)
        break;

      handle_active_access_point(this);
      connected = false;

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
        handle_active_access_point(this);
        connected = false;
        connecting = false;
        for(const auto& listener : listeners)
          listener->handleWifiDisconnected();
      }
  }
}

void WifiStatusNM::handleConnectedAccessPoint() {
  DBG("WifiStatusNM::" << __func__ << " changed active AP");
  connectedAP = getNMConnectedAP(NM_DEVICE_WIFI(nmdevice));
  if (connectedAP)
    DBG("WifiStatusNM::" << __func__ << " ssid = " << connectedAP->ssid);
  else
    DBG("WifiStatusNM::" << __func__ << " connectedAP = NULL");
}

void WifiStatusNM::setConnectedAccessPoint(WifiAccessPoint *ap, String psk) {
  ScopedPointer<StringArray> cmd;
  
  // disconnect if no ap provided
  if (ap == nullptr) {
    NMActiveConnection *conn = nm_device_get_active_connection(nmdevice);

    if (conn) {
      const char *ac_uuid = nm_active_connection_get_uuid(conn);
      const GPtrArray *avail_cons = nm_device_get_available_connections(nmdevice);

      for (int i = 0; avail_cons && (i < avail_cons->len); i++) {
        NMRemoteConnection *candidate = (NMRemoteConnection *) g_ptr_array_index(avail_cons, i);
        const char *test_uuid = nm_connection_get_uuid(NM_CONNECTION(candidate));

        if (g_strcmp0(ac_uuid, test_uuid) == 0) {
          GError *err = NULL;
          nm_remote_connection_delete(candidate, NULL, &err);
          if (err) {
            DBG("WifiStatusNM: failed to remove active connection!");
            DBG("WifiStatusNM::" << __func__ << ": " << err->message);
            g_error_free(err);
          }
          break;
        }
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
    const GByteArray *candidate_ssid;
    //GBytes *candidate_ssid;
    NMAccessPoint *candidate_ap;

    //FIXME: expand WifiAccessPoint struct to know which NMAccessPoint it is
    ap_list = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(nmdevice));
    for (int i = 0; i < ap_list->len; i++) {
      char *ssid;
      candidate_ap = (NMAccessPoint *) g_ptr_array_index(ap_list, i);

      candidate_ssid = nm_access_point_get_ssid(candidate_ap);
      if (!candidate_ssid)
        break;

      ssid = nm_utils_ssid_to_utf8(candidate_ssid);

      if (ssid && ap->ssid == ssid) {
        nm_ap_path = nm_object_get_path(NM_OBJECT(candidate_ap));
        g_free(ssid);
        break;
      }
    }

    if (!nm_ap_path)
      return;

    connecting = true;

    connection = nm_connection_new();
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

    nm_client_add_and_activate_connection(nmclient,
                                                connection,
                                                nmdevice,
                                                nm_ap_path,
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
}

#endif // LINUX
