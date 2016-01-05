#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SwitchComponent.h"
#include "PageStackComponent.h"
#include "TrainComponent.h"

struct WifiAccessPoint {
  String ssid;
  int signalStrength; // -120 to 0
  bool requiresAuth;
};

struct WifiIcons {
  OwnedArray<Drawable> wifiStrength;
  ScopedPointer<Drawable> lockIcon;
};

class WifiAccessPointListItem : public Button {
public:
  WifiAccessPoint ap;

  WifiAccessPointListItem(const WifiAccessPoint &ap, WifiIcons *icons);

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WifiAccessPointListItem)

  WifiIcons *icons;
};

class SettingsPageWifiComponent : public Component, private Button::Listener {
public:
  SettingsPageWifiComponent();
  ~SettingsPageWifiComponent();

  bool wifiEnabled = false;
  bool wifiConnected = false;
  WifiAccessPoint* connectedAp;
  WifiAccessPoint* selectedAp;

  ScopedPointer<PageStackComponent> pageStack;

  ScopedPointer<ImageButton> backButton;
  ScopedPointer<Drawable> wifiIcon;
  ScopedPointer<WifiIcons> icons;

  ScopedPointer<TrainComponent> accessPointListPage;

  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;
  ScopedPointer<TextEditor> passwordEditor;
  ScopedPointer<TextButton> connectionButton;

  ScopedPointer<SwitchComponent> switchComponent;

  var parseWifiListJson(const String &path);

  OwnedArray<WifiAccessPointListItem> accessPointItems;

  void setWifiEnabled(bool enabled);
  void paint(Graphics &g) override;
  void resized() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageWifiComponent)

  void buttonClicked(Button *) override;
  void buttonStateChanged(Button *) override;
};
