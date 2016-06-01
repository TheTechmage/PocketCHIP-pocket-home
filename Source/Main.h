#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "MainComponent.h"
#include "WifiStatus.h"

struct BluetoothDevice {
  String name, macAddress;
  bool connected = false;
  bool paired = false;
};

struct BluetoothStatus {
  OwnedArray<BluetoothDevice> devices;
  bool enabled = false;

  void populateFromJson(const var &json);
};

PageStackComponent &getMainStack();
WifiStatus &getWifiStatus();
BluetoothStatus &getBluetoothStatus();
ImageComponent &getMainButtonPopup();
MainContentComponent &getMainContentComponent();

class PokeLaunchApplication : public JUCEApplication {
public:

#ifdef LINUX
  WifiStatusNM wifiStatusNM;
#else
  WifiStatusJson wifiStatusNM;
#endif //LINUX

  WifiStatusJson wifiStatusJson;
  WifiStatus *wifiStatus;

  BluetoothStatus bluetoothStatus;

  PokeLaunchApplication();

  static PokeLaunchApplication *get();

  bool sound();

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

    void activeWindowStatusChanged() override;
    void closeButtonPressed() override;
  };
  
  PokeLaunchApplication::MainWindow* getMainWindow();

private:
  ScopedPointer<MainWindow> mainWindow;
};
