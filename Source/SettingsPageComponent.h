#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"
#include "IconSliderComponent.h"
#include "TrainComponent.h"
#include "SwitchComponent.h"
#include "SettingsPageWifiComponent.h"
#include "SettingsPageBluetoothComponent.h"

class SettingsCategoryButton : public Button {
public:
  String displayText;

  SettingsCategoryButton(const String &name);
  ~SettingsCategoryButton() {}

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
  void resized() override;

  void setText(const String &text);

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsCategoryButton)
};

class SettingsCategoryItemComponent : public Component, private Button::Listener {
public:
  ScopedPointer<DrawableButton> icon;
  ScopedPointer<Drawable> iconDrawable;
  ScopedPointer<SwitchComponent> toggle;
  ScopedPointer<SettingsCategoryButton> button;

  StretchableLayoutManager layout;

  SettingsCategoryItemComponent(const String &name);
  ~SettingsCategoryItemComponent() {}

  void paint(Graphics &g) override;
  void resized() override;

  void buttonClicked(Button *b) override;
  void buttonStateChanged(Button *b) override;
  void enablementChanged() override;

  virtual void enabledStateChanged(bool enabled) = 0;
  virtual void updateButtonText() = 0;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsCategoryItemComponent)
};

class WifiCategoryItemComponent : public SettingsCategoryItemComponent {
public:
  WifiCategoryItemComponent();

  void enabledStateChanged(bool enabled) override;
  void updateButtonText() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WifiCategoryItemComponent)
};

class BluetoothCategoryItemComponent : public SettingsCategoryItemComponent {
public:
  BluetoothCategoryItemComponent();

  void enabledStateChanged(bool enabled) override;
  void updateButtonText() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BluetoothCategoryItemComponent)
};

class SettingsPageComponent : public Component, private Button::Listener {
public:
  ScopedPointer<IconSliderComponent> screenBrightnessSlider, volumeSlider;
  ScopedPointer<ImageButton> backButton;
  ScopedPointer<Component> mainPage;
  ScopedPointer<WifiCategoryItemComponent> wifiCategoryItem;
  ScopedPointer<BluetoothCategoryItemComponent> bluetoothCategoryItem;

  ScopedPointer<SettingsPageWifiComponent> wifiPage;
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
