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

  ScopedPointer<Drawable> brightLo = Drawable::createFromImageData(
      BinaryData::brightnessIconLo_png, BinaryData::brightnessIconLo_pngSize);
  ScopedPointer<Drawable> brightHi = Drawable::createFromImageData(
      BinaryData::brightnessIconHi_png, BinaryData::brightnessIconHi_pngSize);
  screenBrightnessSlider =
      ScopedPointer<IconSliderComponent>(new IconSliderComponent(*brightLo, *brightHi));

  ScopedPointer<Drawable> volLo =
      Drawable::createFromImageData(BinaryData::volumeIconLo_png, BinaryData::volumeIconLo_pngSize);
  ScopedPointer<Drawable> volHi =
      Drawable::createFromImageData(BinaryData::volumeIconHi_png, BinaryData::volumeIconHi_pngSize);
  volumeSlider = ScopedPointer<IconSliderComponent>(new IconSliderComponent(*volLo, *volHi));

  mainPage->addAndMakeVisible(screenBrightnessSlider);
  mainPage->addAndMakeVisible(volumeSlider);

  sliderLayout.setItemLayout(0, 0.0, -1.0, -1.0);
  sliderLayout.setItemLayout(1, sliderPadding, sliderPadding, sliderPadding);
  sliderLayout.setItemLayout(2, 0.0, -1.0, -1.0);

  // create back button
  ScopedPointer<Drawable> backButtonDrawable =
      Drawable::createFromImageData(BinaryData::backIcon_png, BinaryData::backIcon_pngSize);
  backButton = createImageButtonFromDrawable("Back", *backButtonDrawable);
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  ScopedPointer<Drawable> wifiDrawable =
      Drawable::createFromImageData(BinaryData::wifiIcon_png, BinaryData::wifiIcon_pngSize);
  ScopedPointer<Drawable> bluetoothDrawable = Drawable::createFromImageData(
      BinaryData::bluetoothIcon_png, BinaryData::bluetoothIcon_pngSize);

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

void SettingsPageComponent::paint(Graphics &g) {
  auto bounds = getLocalBounds();
  g.setColour(Colours::black);
  g.fillRect(0, bounds.getBottom() - 62, 62, 62);
}

void SettingsPageComponent::resized() {
  AppsPageComponent::resized();

  auto bounds = getLocalBounds();

  mainPage->setBounds(bounds);

  backButton->setBounds(3, bounds.getBottom() - 56, 50, 50);

  train->centreWithSize(bounds.getWidth(), 96);

  for (auto page : pages) {
    page->setBounds(bounds);
  }

  Component *sliders[] = { screenBrightnessSlider.get(), nullptr, volumeSlider.get() };
  sliderLayout.layOutComponents(sliders, 3, bounds.getX(), bounds.getBottom() - 100 - sliderHeight,
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
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
  }
}
