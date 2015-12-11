#include "SettingsPageComponent.h"
#include "SettingsPageWifiComponent.h"
#include "SettingsPageBluetoothComponent.h"
#include "Main.h"
#include "Utils.h"

static const int sliderHeight = 50;
static const int sliderPadding = 10;

SettingsPageComponent::SettingsPageComponent() {
  mainPage = new Component();
  addAndMakeVisible(mainPage);
  mainPage->toBack();

  screenBrightnessSlider = ScopedPointer<IconSliderComponent>(
      new IconSliderComponent(BinaryData::brightnessIconLo_svg, BinaryData::brightnessIconHi_svg));

  volumeSlider = ScopedPointer<IconSliderComponent>(
      new IconSliderComponent(BinaryData::volumeIconLo_svg, BinaryData::volumeIconHi_svg));

  mainPage->addAndMakeVisible(screenBrightnessSlider);
  mainPage->addAndMakeVisible(volumeSlider);

  sliderLayout.setItemLayout(0, 0.0, -1.0, -1.0);
  sliderLayout.setItemLayout(1, sliderPadding, sliderPadding, sliderPadding);
  sliderLayout.setItemLayout(2, 0.0, -1.0, -1.0);

  ScopedPointer<XmlElement> wifiSvg = XmlDocument::parse(BinaryData::wifiIcon_svg);
  ScopedPointer<XmlElement> bluetoothSvg = XmlDocument::parse(BinaryData::bluetoothIcon_svg);

  ScopedPointer<Drawable> wifiDrawable = Drawable::createFromSVG(*wifiSvg);
  ScopedPointer<Drawable> bluetoothDrawable = Drawable::createFromSVG(*bluetoothSvg);

  auto wifiButton = createImageButtonFromDrawable("WiFi", *wifiDrawable);
  auto bluetoothButton = createImageButtonFromDrawable("Bluetooth", *bluetoothDrawable);

  wifiButton->addListener(this);
  bluetoothButton->addListener(this);

  addAndOwnIcon(wifiButton->getName(), wifiButton);
  addAndOwnIcon(bluetoothButton->getName(), bluetoothButton);

  auto wifiPage = new SettingsPageWifiComponent();
  pages.add(wifiPage);
  pagesByName.set(wifiButton->getName(), wifiPage);

  auto bluetoothPage = new SettingsPageBluetoothComponent();
  pages.add(bluetoothPage);
  pagesByName.set(bluetoothButton->getName(), bluetoothPage);
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {}

void SettingsPageComponent::resized() {
  AppsPageComponent::resized();

  auto bounds = getLocalBounds();

  mainPage->setBounds(bounds);

  train->centreWithSize(bounds.getWidth(), 96);

  bounds.reduce(sliderPadding, sliderPadding);

  for (auto page : pages) {
    page->setBounds(bounds);
  }

  Component *sliders[] = { screenBrightnessSlider.get(), nullptr, volumeSlider.get() };
  sliderLayout.layOutComponents(sliders, 3, bounds.getX(), bounds.getBottom() - sliderHeight,
                                bounds.getWidth(), sliderHeight, false, true);
}

void SettingsPageComponent::buttonClicked(Button *button) {
  for (auto page : pages) {
    page->setVisible(false);
  }
  if (pagesByName.contains(button->getName())) {
    auto page = pagesByName[button->getName()];
    getMainStack().pushPage(page, PageStackComponent::kTransitionTranslateHorizontal);
  }
}
