#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"
#include "IconSliderComponent.h"

class SettingsPageComponent : public AppsPageComponent {
public:
  ScopedPointer<IconSliderComponent> screenBrightnessSlider, volumeSlider;
  StretchableLayoutManager sliderLayout;

  ScopedPointer<Component> mainPage;

  OwnedArray<Component> pages;
  HashMap<String, Component *> pagesByName;

  SettingsPageComponent();
  ~SettingsPageComponent();

  void getDeviceList();

  void paint(Graphics &g) override;
  void resized() override;
  void buttonClicked(Button *b) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageComponent)
};
