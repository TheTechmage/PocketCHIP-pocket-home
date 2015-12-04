#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageComponent.h"
#include "IconSliderComponent.h"

static const int sliderHeight = 50;
static const int sliderPadding = 10;

SettingsPageComponent::SettingsPageComponent() {
  screenBrightnessSlider = ScopedPointer<IconSliderComponent>(new IconSliderComponent(
    BinaryData::brightnessIconLo_svg, BinaryData::brightnessIconHi_svg));

  volumeSlider = ScopedPointer<IconSliderComponent>(new IconSliderComponent(
    BinaryData::volumeIconLo_svg, BinaryData::volumeIconHi_svg));

  addAndMakeVisible(screenBrightnessSlider);
  addAndMakeVisible(volumeSlider);

  sliderLayout.setItemLayout(0, 0.0, -1.0, -1.0);
  sliderLayout.setItemLayout(1, sliderPadding, sliderPadding, sliderPadding);
  sliderLayout.setItemLayout(2, 0.0, -1.0, -1.0);

  {
    wifiButton = new TextButton("WiFi");
    bluetoothButton = new TextButton("Bluetooth");

    train = new TrainComponent();
    train->addAndMakeVisible(wifiButton);
    train->addAndMakeVisible(bluetoothButton);

    addAndMakeVisible(train);
  }
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {}

void SettingsPageComponent::resized() {
  auto bounds = getLocalBounds();

  train->centreWithSize(bounds.getWidth(), 100);

  bounds.reduce(sliderPadding, sliderPadding);

  Component *sliders[] = { screenBrightnessSlider.get(), nullptr, volumeSlider.get() };
  sliderLayout.layOutComponents(sliders, 3, bounds.getX(),
                                bounds.getBottom() - sliderHeight - sliderPadding*2,
                                bounds.getWidth(), sliderHeight, false, true);
}
