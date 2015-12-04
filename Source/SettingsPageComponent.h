#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "IconSliderComponent.h"
#include "TrainComponent.h"

#include <memory>

class SettingsPageComponent : public Component {
public:
  ScopedPointer<IconSliderComponent> screenBrightnessSlider, volumeSlider;
  ScopedPointer<TrainComponent> train;
  ScopedPointer<TextButton> wifiButton, bluetoothButton;

  StretchableLayoutManager sliderLayout;

  SettingsPageComponent();
  ~SettingsPageComponent();

  void paint(Graphics &g);
  void resized();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageComponent)
};
