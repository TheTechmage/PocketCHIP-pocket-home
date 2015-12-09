#include "SettingsPageComponent.h"
#include "SettingsPageWifiComponent.h"
#include "SettingsPageBluetoothComponent.h"

static const int sliderHeight = 50;
static const int sliderPadding = 10;

static ImageButton *createButtonFromDrawable(const String &name, const Drawable &drawable) {
  auto button = new ImageButton(name);
  auto image = Image(Image::RGB, 128, 128, true);
  Graphics g(image);
  drawable.drawWithin(g, Rectangle<float>(0, 0, image.getWidth(), image.getHeight()),
                          RectanglePlacement::fillDestination, 1.0f);
  button->setImages(true, true, true,                       //
                    image, 1.0f, Colours::transparentWhite, // normal
                    image, 1.0f, Colours::transparentWhite, // over
                    image, 0.7f, Colours::transparentBlack, // down
                    0.5f);
  return button;
}


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
  ScopedPointer<XmlElement> backButtonSvg = XmlDocument::parse(BinaryData::backIcon_svg);

  ScopedPointer<Drawable> wifiDrawable = Drawable::createFromSVG(*wifiSvg);
  ScopedPointer<Drawable> bluetoothDrawable = Drawable::createFromSVG(*bluetoothSvg);
  ScopedPointer<Drawable> backButtonDrawable = Drawable::createFromSVG(*backButtonSvg);

  auto wifiButton = createButtonFromDrawable("WiFi", *wifiDrawable);
  auto bluetoothButton = createButtonFromDrawable("Bluetooth", *bluetoothDrawable);
  backButton = createButtonFromDrawable("Back", *backButtonDrawable);

  wifiButton->addListener(this);
  bluetoothButton->addListener(this);
  backButton->addListener(this);

  addAndOwnIcon(wifiButton->getName(), wifiButton);
  addAndOwnIcon(bluetoothButton->getName(), bluetoothButton);

  auto wifiPage = new SettingsPageWifiComponent();
  pages.add(wifiPage);
  pagesByName.set(wifiButton->getName(), wifiPage);
  addChildComponent(wifiPage);

  auto bluetoothPage = new SettingsPageBluetoothComponent();
  pages.add(bluetoothPage);
  pagesByName.set(bluetoothButton->getName(), bluetoothPage);
  addChildComponent(bluetoothPage);

  addChildComponent(backButton);
  backButton->setAlwaysOnTop(true);

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
  sliderLayout.layOutComponents(sliders, 3, bounds.getX(),
                                bounds.getBottom() - sliderHeight - sliderPadding * 2,
                                bounds.getWidth(), sliderHeight, false, true);
}

void SettingsPageComponent::buttonClicked(Button *button) {
  for (auto page : pages) {
    page->setVisible(false);
  }
  if (pagesByName.contains(button->getName())) {
    backButton->setVisible(true);
    train->setVisible(false);
    mainPage->setVisible(false);
    pagesByName[button->getName()]->setVisible(true);
  }
  if (button == backButton) {
    backButton->setVisible(false);
    train->setVisible(true);
    mainPage->setVisible(true);
  }
}
