#include "SettingsPageComponent.h"
#include "Main.h"
#include "Utils.h"

#include <numeric>

SettingsCategoryButton::SettingsCategoryButton(const String &name)
: Button(name), displayText(name) {}

void SettingsCategoryButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bounds = getLocalBounds();

//  g.setColour(findColour(isButtonDown ? TextButton::textColourOnId : TextButton::textColourOffId));
  g.setColour(Colours::black);
  g.setFont(20);
  g.drawText(displayText, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
             Justification::centred);
}

void SettingsCategoryButton::resized() {}

void SettingsCategoryButton::setText(const String &text) {
  displayText = text;
  repaint();
}

SettingsCategoryItemComponent::SettingsCategoryItemComponent(const String &name)
: icon{ new DrawableButton("icon", DrawableButton::ImageFitted) },
  toggle{ new SwitchComponent() },
  button{ new SettingsCategoryButton(name) } {
  toggle->addListener(this);
  addAndMakeVisible(icon);
  addAndMakeVisible(toggle);
  addAndMakeVisible(button);
  button->setEnabled(false); // default to disabled state
}

void SettingsCategoryItemComponent::paint(Graphics &g) {}

void SettingsCategoryItemComponent::resized() {
  auto b = getLocalBounds();
  auto h = b.getHeight();

  int spacing = 10;
  int togWidth = h * 1.1f;

  layout.setItemLayout(0, h, h, h);
  layout.setItemLayout(1, spacing, spacing, spacing);
  layout.setItemLayout(2, togWidth, togWidth, togWidth);
  layout.setItemLayout(3, spacing, spacing, spacing);
  layout.setItemLayout(4, h, -1, -1);

  Component *comps[] = { icon, nullptr, toggle, nullptr, button };
  layout.layOutComponents(comps, 5, b.getX(), b.getY(), b.getWidth(), b.getHeight(), false, true);
}

void SettingsCategoryItemComponent::buttonClicked(Button *b) {}

void SettingsCategoryItemComponent::buttonStateChanged(Button *b) {
  if (b == toggle) {
    enabledStateChanged(toggle->getToggleState());
  }
}

void SettingsCategoryItemComponent::enablementChanged() {
  updateButtonText();
}

WifiCategoryItemComponent::WifiCategoryItemComponent() : SettingsCategoryItemComponent("wifi") {
  iconDrawable =
      Drawable::createFromImageData(BinaryData::wifiIcon_png, BinaryData::wifiIcon_pngSize);
  icon->setImages(iconDrawable);
  updateButtonText();
}

void WifiCategoryItemComponent::enabledStateChanged(bool enabled) {
  getWifiStatus().enabled = enabled;
  button->setEnabled(enabled);
  updateButtonText();
}

void WifiCategoryItemComponent::updateButtonText() {
  const auto &status = getWifiStatus();
  if (status.enabled) {
    button->setText(status.connected ? status.connectedAccessPoint->ssid : "Not Connected");
  } else {
    button->setText("WiFi Off");
  }
}

BluetoothCategoryItemComponent::BluetoothCategoryItemComponent()
: SettingsCategoryItemComponent("bluetooth") {
  iconDrawable = Drawable::createFromImageData(BinaryData::bluetoothIcon_png,
                                               BinaryData::bluetoothIcon_pngSize);
  icon->setImages(iconDrawable);
  updateButtonText();
}

void BluetoothCategoryItemComponent::enabledStateChanged(bool enabled) {
  getBluetoothStatus().enabled = enabled;
  button->setEnabled(enabled);
  updateButtonText();
}

void BluetoothCategoryItemComponent::updateButtonText() {
  const auto &status = getBluetoothStatus();
  if (status.enabled) {
    int connectedDeviceCount =
        std::accumulate(status.devices.begin(), status.devices.end(), 0,
                        [](int n, BluetoothDevice *d) { return n + d->connected; });
    if (connectedDeviceCount > 0) {
      button->setText(std::to_string(connectedDeviceCount) + " Devices Connected");
    } else {
      button->setText("No Devices Connected");
    }
  } else {
    button->setText("Bluetooth Off");
  }
}

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

  // create back button
  ScopedPointer<Drawable> backButtonDrawable =
      Drawable::createFromImageData(BinaryData::backIcon_png, BinaryData::backIcon_pngSize);
  backButton = createImageButtonFromDrawable("Back", *backButtonDrawable);
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  wifiCategoryItem = new WifiCategoryItemComponent();
  wifiCategoryItem->button->addListener(this);
  addAndMakeVisible(wifiCategoryItem);

  bluetoothCategoryItem = new BluetoothCategoryItemComponent();
  bluetoothCategoryItem->button->addListener(this);
  addAndMakeVisible(bluetoothCategoryItem);

  addAndMakeVisible(screenBrightnessSlider);
  addAndMakeVisible(volumeSlider);

  wifiPage = new SettingsPageWifiComponent();
  bluetoothPage = new SettingsPageBluetoothComponent();
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {}

void SettingsPageComponent::resized() {
  auto bounds = getLocalBounds();

  {
    for (int i = 0, j = 0; i < 4; ++i) {
      if (i > 0) verticalLayout.setItemLayout(j++, 0, -1, -1);
      verticalLayout.setItemLayout(j++, 48, 48, 48);
    }

    Component *settingsItems[] = { wifiCategoryItem,       nullptr, bluetoothCategoryItem, nullptr,
                                   screenBrightnessSlider, nullptr, volumeSlider };
    auto b = bounds.reduced(10);
    b.setLeft(70);
    verticalLayout.layOutComponents(settingsItems, 7, b.getX(), b.getY(), b.getWidth(),
                                    b.getHeight(), true, true);
  }

  mainPage->setBounds(bounds);

  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());
}

void SettingsPageComponent::buttonClicked(Button *button) {
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
  } else if (button == wifiCategoryItem->button) {
    const auto &wifistatus = getWifiStatus();
    if (wifistatus.connected) {
      wifiPage->pageStack->swapPage(wifiPage->connectionPage, PageStackComponent::kTransitionNone);
    }
    getMainStack().pushPage(wifiPage, PageStackComponent::kTransitionTranslateHorizontal);
  } else if (button == bluetoothCategoryItem->button) {
    getMainStack().pushPage(bluetoothPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}
