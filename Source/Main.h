#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MainComponent.h"
#include "Utils.h"

struct WifiAccessPoint {
  String ssid;
  int signalStrength; // -120 to 0
  bool requiresAuth;
};

struct WifiStatus {
  OwnedArray<WifiAccessPoint> accessPoints;
  WifiAccessPoint *connectedAccessPoint = nullptr;
  bool enabled = false;
  bool connected = false;

  void setConnectedAccessPoint(WifiAccessPoint *ap);
  void setDisconnected();

  void populateFromJson(const var &json);
};

PageStackComponent &getMainStack();
WifiStatus &getWifiStatus();

class PokeLaunchApplication : public JUCEApplication {
public:
  WifiStatus wifiStatus;

  PokeLaunchApplication();

  static PokeLaunchApplication *get();

  const String getApplicationName() override;
  const String getApplicationVersion() override;
  bool moreThanOneInstanceAllowed() override;

  void initialise(const String &commandLine) override;
  void shutdown() override;

  void systemRequestedQuit() override;
  void anotherInstanceStarted(const String &commandLine) override;

  PageStackComponent &getMainStack();

  class MainWindow : public DocumentWindow {
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)

  public:
    MainWindow(String name, const var &configJson);

    void closeButtonPressed() override;
  };

private:
  ScopedPointer<MainWindow> mainWindow;
};
