#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"
#include "IconSliderComponent.h"

class SettingsPageComponent : public AppsPageComponent {
public:
  ScopedPointer<IconSliderComponent> screenBrightnessSlider, volumeSlider;
  StretchableLayoutManager sliderLayout;

  SettingsPageComponent();
  ~SettingsPageComponent();

  void paint(Graphics &g) override;
  void resized() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageComponent)
};
