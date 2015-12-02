#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageComponent.h"

static const int sliderHeight = 60;

SettingsPageComponent::SettingsPageComponent() {
  auto createSlider = [&] {
    auto s = new Slider();
    s->setSliderStyle(Slider::LinearHorizontal);
    s->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    s->setBounds(0, 0, 0, sliderHeight);
    return std::unique_ptr<Slider>(s);
  };

  screenBrightnessSlider = createSlider();
  volumeSlider = createSlider();

  addAndMakeVisible(screenBrightnessSlider.get());
  addAndMakeVisible(volumeSlider.get());

  sliderLayout.setItemLayout(0, -0.5, -0.5, -0.5);
  sliderLayout.setItemLayout(1, -0.5, -0.5, -0.5);
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {}

void SettingsPageComponent::resized() {
  auto bounds = getLocalBounds().reduced(10);

  Component *sliders[] = { screenBrightnessSlider.get(), volumeSlider.get() };
  sliderLayout.layOutComponents(sliders, 2, bounds.getX(), bounds.getBottom() - sliderHeight,
                                bounds.getWidth(), sliderHeight, false, false);
}
