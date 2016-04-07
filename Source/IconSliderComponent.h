#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SettingsPageComponent;

class IconSliderComponent : public Component, public Slider::Listener {
public:
  ScopedPointer<Slider> slider;
  ScopedPointer<DrawableButton> iconLow, iconHi;

  StretchableLayoutManager sliderLayout;

  IconSliderComponent(const Drawable &lo, const Drawable &hi);
  ~IconSliderComponent();

  void paint(Graphics &) override;
  void resized() override;
  virtual void sliderValueChanged(Slider*) override;
  virtual void sliderDragStarted(Slider*) override;
  virtual void sliderDragEnded(Slider*) override;
  
  void addListener(SettingsPageComponent*);

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconSliderComponent)
  
  SettingsPageComponent* listener;
};
