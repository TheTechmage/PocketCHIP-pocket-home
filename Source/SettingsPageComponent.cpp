#include "SettingsPageComponent.h"
#include "Main.h"
#include "Utils.h"
#include "PokeLookAndFeel.h"

#include <numeric>

void SettingsBrightnessTimer::timerCallback() {
  if (settingsPage) {
    settingsPage->setScreenBrightness();
  }
}

void SettingsVolumeTimer::timerCallback() {
  if (settingsPage) {
    settingsPage->setSoundVolume();
  }
}

SettingsCategoryButton::SettingsCategoryButton(const String &name)
: Button(name),
  displayText(name)
{ }

void SettingsCategoryButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  const auto& bounds = getLocalBounds();
  float borderThick = 4.0f;
  float radius = float(bounds.getHeight()) / 2.0f;
  
  g.setColour(Colours::white);
  if (isEnabled()) {
    g.drawRoundedRectangle(bounds.getX() + borderThick, bounds.getY() + borderThick,
                           bounds.getWidth() - 2*borderThick, bounds.getHeight()  - 2*borderThick,
                           radius, borderThick);
  }
  
  // TODO: write button text as grey if choice is completely unset?
  g.setFont(20);
  g.drawText(displayText, bounds.getX(), bounds.getY(),
             bounds.getWidth(), bounds.getHeight(),
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
  toggle->setTriggeredOnMouseDown(true);
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
  bgColor = Colour(0xffd23c6d);
  mainPage = new Component();
  addAndMakeVisible(mainPage);
  mainPage->toBack();
  ChildProcess child;

  // Get initial brightness value
#if JUCE_LINUX
  // TODO: perform this check without blocking against a shell command
  DBG("Set initial brightness via shell");
  if(child.start("cat /sys/class/backlight/backlight/brightness")) {
    String result{child.readAllProcessOutput()};
    brightness = result.getIntValue();
  }
#else
  brightness = 8;
#endif

  // Set initial volume value
  volume = 90;
  //#if JUCE_LINUX
  //if(child.start("amixer get Master")) {
    //const String result (child.readAllProcessOutput());
    //child.waitForProcessToFinish (5 * 1000);
    //volume = atoi(result);
 // }
//#else
//  brightness = 8;
//#endif
//  volume = 100;

  ScopedPointer<Drawable> brightLo = Drawable::createFromImageData(
      BinaryData::brightnessIconLo_png, BinaryData::brightnessIconLo_pngSize);
  ScopedPointer<Drawable> brightHi = Drawable::createFromImageData(
      BinaryData::brightnessIconHi_png, BinaryData::brightnessIconHi_pngSize);
  screenBrightnessSlider =
      ScopedPointer<IconSliderComponent>(new IconSliderComponent(*brightLo, *brightHi));
  screenBrightnessSlider->addListener(this);
  screenBrightnessSlider->slider->setValue((brightness-0.1)*10);

  ScopedPointer<Drawable> volLo =
      Drawable::createFromImageData(BinaryData::volumeIconLo_png, BinaryData::volumeIconLo_pngSize);
  ScopedPointer<Drawable> volHi =
      Drawable::createFromImageData(BinaryData::volumeIconHi_png, BinaryData::volumeIconHi_pngSize);
  volumeSlider = ScopedPointer<IconSliderComponent>(new IconSliderComponent(*volLo, *volHi));
  volumeSlider->addListener(this);
  volumeSlider->slider->setValue(volume);

  // create back button
  backButton = createImageButton(
      "Back", ImageFileFormat::loadFrom(BinaryData::backIcon_png, BinaryData::backIcon_pngSize));
  backButton->addListener(this);
  backButton->setTriggeredOnMouseDown(true);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  wifiCategoryItem = new WifiCategoryItemComponent();
  wifiCategoryItem->button->setTriggeredOnMouseDown(true);
  wifiCategoryItem->button->addListener(this);
  addAndMakeVisible(wifiCategoryItem);

  bluetoothCategoryItem = new BluetoothCategoryItemComponent();
  bluetoothCategoryItem->button->setTriggeredOnMouseDown(true);
  bluetoothCategoryItem->button->addListener(this);
  addAndMakeVisible(bluetoothCategoryItem);

  addAndMakeVisible(screenBrightnessSlider);
  addAndMakeVisible(volumeSlider);

  wifiPage = new SettingsPageWifiComponent();
  bluetoothPage = new SettingsPageBluetoothComponent();
  
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {
    g.fillAll(bgColor);
}

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

void SettingsPageComponent::setSoundVolume() {
  DBG("set vol");
  volume = volumeSlider->slider->getValue();
#if JUCE_LINUX
  child.start("amixer cset numid=1 100%");
#endif
}

void SettingsPageComponent::setScreenBrightness() {
  DBG("set bright");
  brightness = 1+(screenBrightnessSlider->slider->getValue()*0.09);
#if JUCE_LINUX
  child.start("echo 8 > /sys/class/backlight/backlight/brightness");
#endif
}


void SettingsPageComponent::sliderValueChanged(IconSliderComponent* slider) {
  //
}

void SettingsPageComponent::sliderDragStarted(IconSliderComponent* slider) {
  if( slider == screenBrightnessSlider && !brightnessSliderTimer.isTimerRunning()) {
    brightnessSliderTimer.startTimer(200);
    brightnessSliderTimer.settingsPage = this;
  } else if( slider == volumeSlider&& !volumeSliderTimer.isTimerRunning()) {
    volumeSliderTimer.startTimer(200);
    volumeSliderTimer.settingsPage = this;
  }
}

void SettingsPageComponent::sliderDragEnded(IconSliderComponent* slider) {
  if( slider == screenBrightnessSlider && brightnessSliderTimer.isTimerRunning()) {
    brightnessSliderTimer.stopTimer();
    setScreenBrightness();
  } else if( slider == volumeSlider&& volumeSliderTimer.isTimerRunning()) {
    volumeSliderTimer.stopTimer();
    setSoundVolume();
  }
}
