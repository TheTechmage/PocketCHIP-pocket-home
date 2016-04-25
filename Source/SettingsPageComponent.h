#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"
#include "IconSliderComponent.h"
#include "TrainComponent.h"
#include "SwitchComponent.h"
#include "SettingsPageWifiComponent.h"
#include "SettingsPageBluetoothComponent.h"
#include "WifiStatus.h"

class SettingsPageComponent;

class SettingsBrightnessTimer : public Timer {
public:
  SettingsBrightnessTimer() {};
  virtual void timerCallback() override;
  SettingsPageComponent* settingsPage;
};

class SettingsVolumeTimer : public Timer {
public:
  SettingsVolumeTimer() {};
  virtual void timerCallback() override;
  SettingsPageComponent* settingsPage;
};

class SettingsCategoryButton : public Button {
public:
  String displayText;

  SettingsCategoryButton(const String &name);
  ~SettingsCategoryButton() {}

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
  void resized() override;

  void setText(const String &text);

private:
  Rectangle<int> pillBounds;
  
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
  void enablementChanged() override;

  virtual void enabledStateChanged(bool enabled) = 0;
  virtual void updateButtonText() = 0;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsCategoryItemComponent)
};

class WifiCategoryItemComponent : public SettingsCategoryItemComponent, public WifiStatus::Listener {
public:
  WifiCategoryItemComponent();

  void enabledStateChanged(bool enabled) override;
  void updateButtonText() override;
  
  void handleWifiEnabled() override;
  void handleWifiDisabled() override;
  void handleWifiConnected() override;
  void handleWifiDisconnected() override;

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

  ScopedPointer<SettingsPageWifiComponent> wifiPage;

  StretchableLayoutManager verticalLayout;

  SettingsPageComponent();
  ~SettingsPageComponent();

  void getDeviceList();

  void paint(Graphics &g) override;
  void resized() override;

  void buttonClicked(Button *b) override;
  
  void setSoundVolume();
  void setScreenBrightness();
  
  void sliderValueChanged(IconSliderComponent* slider);
  void sliderDragStarted(IconSliderComponent* slider);
  void sliderDragEnded(IconSliderComponent* slider);
    
  void visibilityChanged() override;

  void checkSliders();
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageComponent)
  Colour bgColor;
  Image bgImage;
  SettingsBrightnessTimer brightnessSliderTimer;
  SettingsVolumeTimer volumeSliderTimer;
  ChildProcess child;
  unsigned int brightness;
  unsigned int volume;
};

