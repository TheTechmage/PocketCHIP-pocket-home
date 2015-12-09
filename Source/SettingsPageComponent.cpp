#include "SettingsPageComponent.h"

static const int sliderHeight = 50;
static const int sliderPadding = 10;

SettingsPageComponent::SettingsPageComponent() {
  screenBrightnessSlider = ScopedPointer<IconSliderComponent>(
      new IconSliderComponent(BinaryData::brightnessIconLo_svg, BinaryData::brightnessIconHi_svg));

  volumeSlider = ScopedPointer<IconSliderComponent>(
      new IconSliderComponent(BinaryData::volumeIconLo_svg, BinaryData::volumeIconHi_svg));

  addAndMakeVisible(screenBrightnessSlider);
  addAndMakeVisible(volumeSlider);

  sliderLayout.setItemLayout(0, 0.0, -1.0, -1.0);
  sliderLayout.setItemLayout(1, sliderPadding, sliderPadding, sliderPadding);
  sliderLayout.setItemLayout(2, 0.0, -1.0, -1.0);

  ScopedPointer<XmlElement> wifiSvg = XmlDocument::parse(BinaryData::wifiIcon_svg);
  ScopedPointer<XmlElement> bluetoothSvg = XmlDocument::parse(BinaryData::bluetoothIcon_svg);

  addAndOwnIcon("WiFi", Drawable::createFromSVG(*wifiSvg));
  addAndOwnIcon("Bluetooth", Drawable::createFromSVG(*bluetoothSvg));
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {}

void SettingsPageComponent::resized() {
  AppsPageComponent::resized();

  auto bounds = getLocalBounds();

  train->centreWithSize(bounds.getWidth(), 96);

  bounds.reduce(sliderPadding, sliderPadding);

  Component *sliders[] = { screenBrightnessSlider.get(), nullptr, volumeSlider.get() };
  sliderLayout.layOutComponents(sliders, 3, bounds.getX(),
                                bounds.getBottom() - sliderHeight - sliderPadding * 2,
                                bounds.getWidth(), sliderHeight, false, true);
}
