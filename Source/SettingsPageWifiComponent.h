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

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WifiAccessPointListItem)

  WifiIcons *icons;
};

class SettingsPageWifiComponent : public Component, private Button::Listener {
public:
  SettingsPageWifiComponent();
  ~SettingsPageWifiComponent();

  bool init = false;

  WifiAccessPoint *selectedAp;

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

  void setWifiEnabled(bool enabled);
  void paint(Graphics &g) override;
  void resized() override;

private:
  Colour bgColor;

  void buttonClicked(Button *) override;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageWifiComponent)
};
