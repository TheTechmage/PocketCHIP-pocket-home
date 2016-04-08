#include "WifiStatusNM.h"

File absoluteFileFromPath(const String &path) {
  return File::isAbsolutePath(path) ? File(path)
                                    : File::getCurrentWorkingDirectory().getChildFile(path);
}

// TODO: allow user overrides of asset files
File assetFile(const String &fileName) {
  auto devFile = absoluteFileFromPath("../../assets/" + fileName);
  File assetFile;

  assetFile = devFile;
  
  return assetFile;
}

class TestWifiStatus {
public:
  WifiStatusNM wifiStatus;

  WifiStatus &getWifiStatus();

  void test_populate();
  void test_ap_list();
  void test_enabled();
  void test_connected();
  WifiAccessPoint * test_find_ap(String *ssid);
  void test_set_disabled();
  void test_set_enabled();
  void test_set_connected(WifiAccessPoint *ap, const String psk);
  void test_set_disconnected();
};

WifiStatus &TestWifiStatus::getWifiStatus() {
  return this->wifiStatus;
}

void TestWifiStatus::test_populate() {
  // Populate with dummy data
  std::cout << "Populating wifiStatus from wifi.json ..." << std::endl;
  auto ssidListFile = assetFile("wifi.json");
  getWifiStatus().initializeStatus();
}

void TestWifiStatus::test_ap_list() {
  std::cout << "Printing known AP's from wifiStatus ..." << std::endl;
  for (auto ap : *getWifiStatus().nearbyAccessPoints()) {
	  std::cout << ap->ssid << std::endl;
  }
}

void TestWifiStatus::test_enabled() {
  std::cout << "Is wifiStatus enabled?" << std::endl;
  std::cout << getWifiStatus().isEnabled() << std::endl;
}

void TestWifiStatus::test_connected() {
  std::cout << "Is wifiStatus connected?" << std::endl;
  std::cout << getWifiStatus().isConnected() << std::endl;
}

WifiAccessPoint * TestWifiStatus::test_find_ap(String *ssid) {
  std::cout << "Finding " << *ssid << " within wifiStatus..." << std::endl;
  for (auto ap : *getWifiStatus().nearbyAccessPoints()) {
    if (ap->ssid == *ssid) {
      return ap;
    }
  }
  std::cout << "Failed searching for " << *ssid << std::endl;
  return NULL;
}

void TestWifiStatus::test_set_enabled() {
  std::cout << "Enabling wifiStatus ..." << std::endl;
  getWifiStatus().setEnabled();
}

void TestWifiStatus::test_set_disabled() {
  std::cout << "Disabling wifiStatus ..." << std::endl;
  getWifiStatus().setDisabled();
}

void TestWifiStatus::test_set_connected(WifiAccessPoint *ap, const String psk) {
  std::cout << "Connecting wifiStatus to " << ap->ssid << std::endl;
  getWifiStatus().setConnectedAccessPoint(ap, psk);
}

void TestWifiStatus::test_set_disconnected() {
  std::cout << "Disconnecting wifiStatus ..." << std::endl;
  getWifiStatus().setDisconnected();
}

int main() {
  TestWifiStatus wifi;
  WifiAccessPoint *ap;
  String ssid = "BOGUS_SSID";
  String psk = "BOGUS_PSK";

  wifi.test_set_enabled();
  wifi.test_enabled();
  wifi.test_connected();

  wifi.test_set_disabled();
  wifi.test_enabled();
  wifi.test_connected();

  wifi.test_set_enabled();
  wifi.test_enabled();
  wifi.test_connected();

  wifi.test_populate();
  wifi.test_ap_list();
  wifi.test_enabled();
  wifi.test_connected();

  ap = wifi.test_find_ap(&ssid);
  wifi.test_set_connected(ap, psk);
  wifi.test_enabled();
  wifi.test_connected();

  //wifi.test_set_disconnected();
  wifi.test_enabled();
  wifi.test_connected();

}
