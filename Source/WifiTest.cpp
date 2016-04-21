#include "../JuceLibraryCode/JuceHeader.h"
#include "WifiStatus.h"

class WifiTestListener : public WifiStatus::Listener {
public:
  WifiTestListener();

  bool expectEnabled = false;
  bool expectConnected = false;

  void handleWifiEnabled() override;
  void handleWifiDisabled() override;
  void handleWifiConnected() override;
  void handleWifiDisconnected() override;
  void handleWifiFailedConnect() override;
};

WifiTestListener::WifiTestListener() : expectEnabled(), expectConnected() {}

void WifiTestListener::handleWifiEnabled() {
  DBG(__func__);
  if (!expectEnabled)
    std::cerr << "FAILED! not expecting to become enabled" << std::endl;
}

void WifiTestListener::handleWifiDisabled() {
  DBG(__func__);
  if (expectEnabled)
    std::cerr << "FAILED! not expecting to become disabled" << std::endl;
}

void WifiTestListener::handleWifiConnected() {
  DBG(__func__);
  if (!expectConnected)
    std::cerr << "FAILED! not expecting to become connected" << std::endl;
}

void WifiTestListener::handleWifiDisconnected() {
  DBG(__func__);
  if (expectConnected)
    std::cerr << "FAILED! not expecting to become disconnected" << std::endl;
}

void WifiTestListener::handleWifiFailedConnect() {
  DBG(__func__);
  if (expectConnected)
    std::cerr << "FAILED! not expecting to connection failure" << std::endl;
}

class WifiTestApplication;

class WifiNextStepTimer : public Timer {
public:
  WifiNextStepTimer();
  ~WifiNextStepTimer();

  void timerCallback() override;

  WifiTestApplication *wifiTestApp;
};

WifiNextStepTimer::WifiNextStepTimer() : wifiTestApp() {}
WifiNextStepTimer::~WifiNextStepTimer() {}

class WifiTestApplication : public JUCEApplication {
public:
  const String getApplicationName() override;
  const String getApplicationVersion() override;
  void shutdown() override;

  WifiStatusNM wifiStatusNM;
  WifiStatusJson wifiStatusJson;
  WifiStatus *wifiStatus;

  WifiStatus &getWifiStatus();

  int currentStep = 0;
  void nextTestStep();

  String ssid = "BOGUS_SSID";
  String psk = "";
  void initialise(const String &commandLine);
  
  WifiTestListener *createWifiTestListener();

private:
  Array< std::function<void(WifiStatus *wifiStatus)> > testSteps;
  WifiNextStepTimer testStepTimer;
  OwnedArray<WifiTestListener> listeners;
};

void WifiNextStepTimer::timerCallback() {
  DBG(__func__);
  if (wifiTestApp) {
    wifiTestApp->nextTestStep();
  }
}

WifiTestListener *WifiTestApplication::createWifiTestListener() {
  auto listener = new WifiTestListener();
  listeners.add(listener);
  return listener;
}

void WifiTestApplication::nextTestStep() {
  std::cout << std::endl << std::endl;
  std::cout << "Executing testStep" << currentStep << std::endl;
  if (currentStep < testSteps.size()) {
    wifiStatus->clearListeners();
    testSteps[currentStep](wifiStatus);
    currentStep++;
    return;
  }

  testStepTimer.stopTimer();
  quit();
}


void WifiTestApplication::initialise(const String &commandLine) {
  StringArray args;
  args.addTokens(commandLine, true);

  if (args.contains("--help")) {
    std::cerr << "arguments:" << std::endl;
    std::cerr << "  --help:      Print usage help" << std::endl;
    std::cerr << "  --fakewifi:  Use fake WifiStatus" << std::endl;
    std::cerr << "  --ssid <SSID>" << std::endl;
    std::cerr << "  --psk <PSK>" << std::endl;
    quit();
  }

  if (args.contains("--fakewifi"))
    wifiStatus = &wifiStatusJson;
  else
    wifiStatus = &wifiStatusNM;

  if (args.contains("--ssid"))
    ssid = args[args.indexOf("--ssid")+1].unquoted();
  if (args.contains("--psk"))
    psk = args[args.indexOf("--psk")+1].unquoted();

  std::cout << "Using SSID = " << ssid << std::endl;
  std::cout << "Using PSK = " << psk << std::endl;
  
  auto test_set_enabled = [this](WifiStatus *wifiStatus) {
    std::cout << "Enabling wifiStatus ..." << std::endl;

    auto listener = this->createWifiTestListener();
    listener->expectEnabled = true;
    listener->expectConnected = false;
    wifiStatus->addListener(listener);

    wifiStatus->setEnabled();
  };

  auto test_set_disabled = [this](WifiStatus *wifiStatus) {
    std::cout << "Disabling wifiStatus ..." << std::endl;

    auto listener = this->createWifiTestListener();
    listener->expectEnabled = false;
    listener->expectConnected = false;
    wifiStatus->addListener(listener);

    wifiStatus->setDisabled();
  };

  auto test_initialize = [](WifiStatus *wifiStatus) {
    std::cout << "Initializing wifiStatus ..." << std::endl;
    wifiStatus->initializeStatus();
  };

  auto test_ap_list = [](WifiStatus *wifiStatus) {
    std::cout << "Printing known AP's from wifiStatus ..." << std::endl;
    for (auto ap : wifiStatus->nearbyAccessPoints()) {
  	  std::cout << ap->ssid << std::endl;
    }
  };

  auto test_enabled = [](WifiStatus *wifiStatus) {
    std::cout << "Is wifiStatus enabled?" << std::endl;
    std::cout << wifiStatus->isEnabled() << std::endl;
  };

  auto test_connected = [](WifiStatus *wifiStatus) {
    std::cout << "Is wifiStatus connected?" << std::endl;
    std::cout << wifiStatus->isConnected() << std::endl;
  };

  auto test_find_and_connect = [this](WifiStatus *wifiStatus) {
    std::cout << "Finding SSID " << ssid << " within wifiStatus..." << std::endl;

    auto listener = this->createWifiTestListener();
    listener->expectEnabled = true;
    listener->expectConnected = true;
    wifiStatus->addListener(listener);

    for (auto ap : wifiStatus->nearbyAccessPoints()) {
      std::cout << "Comparing: " << ap->ssid << " to " << ssid << std::endl;
      if (ap->ssid == ssid) {
	std::cout << "Found: " << ap->ssid << std::endl;
        std::cout << "Connecting wifiStatus to " << ap->ssid << std::endl;
        wifiStatus->setConnectedAccessPoint(ap, psk);
        return;
      }
    }
    std::cout << "Failed searching for " << ssid << std::endl;
  };

  auto test_connected_ap = [this](WifiStatus *wifiStatus) {
    auto ap = wifiStatus->connectedAccessPoint();
    std::cout << "Connected AP SSID = " << ap.ssid << std::endl;
  };

  auto test_explicit_disconnect = [this](WifiStatus *wifiStatus) {
    std::cout << "Disconnecting wifiStatus..." << std::endl;

    auto listener = this->createWifiTestListener();
    listener->expectEnabled = false;
    listener->expectConnected = false;
    wifiStatus->addListener(listener);

    wifiStatus->setDisconnected();
  };

  testSteps.add(test_initialize);
  testSteps.add(test_enabled);
  testSteps.add(test_connected);
  testSteps.add(test_set_enabled);
  testSteps.add(test_enabled);
  testSteps.add(test_connected);
  testSteps.add(test_ap_list);
  testSteps.add(test_find_and_connect);
  testSteps.add(test_enabled);
  testSteps.add(test_connected);
  testSteps.add(test_connected_ap);
  testSteps.add(test_explicit_disconnect);
  testSteps.add(test_enabled);
  testSteps.add(test_connected);
  testSteps.add(test_set_disabled);
  testSteps.add(test_enabled);
  testSteps.add(test_connected);

  testStepTimer.wifiTestApp = this;
  testStepTimer.startTimer(2000);
}

WifiStatus &WifiTestApplication::getWifiStatus() {
  return *this->wifiStatus;
}

void WifiTestApplication::shutdown() {
}


const String WifiTestApplication::getApplicationName() {
  return "wifitest Application";
}

const String WifiTestApplication::getApplicationVersion() {
  return "1.0";
}

START_JUCE_APPLICATION(WifiTestApplication)
