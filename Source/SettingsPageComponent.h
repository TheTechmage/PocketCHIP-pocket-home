#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"
#include "IconSliderComponent.h"
#include "TrainComponent.h"
#include "SwitchComponent.h"
#include "SettingsPageWifiComponent.h"
#include "SettingsPageBluetoothComponent.h"

class SettingsCategoryItemComponent : public Component, private Button::Listener {
public:
  ScopedPointer<DrawableButton> icon;
  ScopedPointer<SwitchComponent> toggle;
  ScopedPointer<TextButton> button;

  StretchableLayoutManager layout;

  SettingsCategoryItemComponent(const Drawable *iconImage);
  ~SettingsCategoryItemComponent() {}

  void paint(Graphics &g) override;
  void resized() override;

  void buttonClicked(Button *b) override;
  void buttonStateChanged(Button *b) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsCategoryItemComponent)
};

class SettingsPageComponent : public Component, private Button::Listener {
public:
  ScopedPointer<IconSliderComponent> screenBrightnessSlider, volumeSlider;
  ScopedPointer<ImageButton> backButton;
  ScopedPointer<Component> mainPage;
  ScopedPointer<SettingsCategoryItemComponent> wifiCategoryItem;
  ScopedPointer<SettingsCategoryItemComponent> bluetoothCategoryItem;

  ScopedPointer<Drawable> wifiIcon;
  ScopedPointer<SettingsPageWifiComponent> wifiPage;
  ScopedPointer<Drawable> bluetoothIcon;
  ScopedPointer<SettingsPageBluetoothComponent> bluetoothPage;

  StretchableLayoutManager verticalLayout;

  SettingsPageComponent();
  ~SettingsPageComponent();

  void getDeviceList();

  void paint(Graphics &g) override;
  void resized() override;

  void buttonClicked(Button *b) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageComponent)
};
