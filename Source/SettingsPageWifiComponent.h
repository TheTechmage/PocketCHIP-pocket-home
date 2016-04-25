#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SwitchComponent.h"
#include "PageStackComponent.h"
#include "TrainComponent.h"
#include "Main.h"

struct WifiIcons {
  OwnedArray<Drawable> wifiStrength;
  ScopedPointer<Drawable> lockIcon;
  ScopedPointer<Drawable> arrowIcon;
};

class WifiAccessPointListItem : public Button {
public:
  WifiAccessPoint *ap;

  WifiAccessPointListItem(WifiAccessPoint *ap, WifiIcons *icons);

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
  void resized() override;

private:
  WifiIcons *icons;

  int wifiSignalStrengthToIdx(int strength);
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WifiAccessPointListItem)
};

class SettingsPageWifiComponent : public Component, public WifiStatus::Listener, private Button::Listener {
public:
  SettingsPageWifiComponent();
  ~SettingsPageWifiComponent();

  WifiAccessPoint selectedAp;

  ScopedPointer<PageStackComponent> pageStack;

  ScopedPointer<ImageButton> backButton;
  ScopedPointer<ImageComponent> wifiIconComponent;
  ScopedPointer<WifiIcons> icons;

  ScopedPointer<TrainComponent> accessPointListPage;

  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;
  ScopedPointer<TextEditor> passwordEditor;
  ScopedPointer<TextButton> connectionButton;

  ScopedPointer<SwitchComponent> switchComponent;

  OwnedArray<WifiAccessPointListItem> accessPointItems;

  void paint(Graphics &g) override;
  void resized() override;
  
  void handleWifiDisabled() override;
  void handleWifiConnected() override;
  void handleWifiDisconnected() override;
  void handleWifiFailedConnect() override;

private:
  Colour bgColor;
  Image bgImage;
  
  bool init = false;

  void buttonClicked(Button *) override;
  void updateAccessPoints();

  OwnedArray<WifiAccessPoint> accessPoints;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageWifiComponent)
};
