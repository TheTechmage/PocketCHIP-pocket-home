#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageComponent.h"

static const int sliderHeight = 50;
static const int sliderPadding = 10;

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

  sliderLayout.setItemLayout(0, 0.0, -1.0, -1.0);
  sliderLayout.setItemLayout(1, sliderPadding, sliderPadding, sliderPadding);
  sliderLayout.setItemLayout(2, 0.0, -1.0, -1.0);
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {}

void SettingsPageComponent::resized() {
  auto bounds = getLocalBounds().reduced(sliderPadding);

  Component *sliders[] = { screenBrightnessSlider.get(), nullptr, volumeSlider.get() };
  sliderLayout.layOutComponents(sliders, 3, bounds.getX(), bounds.getBottom() - sliderHeight,
                                bounds.getWidth(), sliderHeight, false, true);
}
