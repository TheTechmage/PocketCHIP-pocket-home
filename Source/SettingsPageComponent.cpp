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
{}

void SettingsCategoryButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  const auto& bounds = pillBounds;
  float borderThick = pillBounds.getHeight() / 10.f;
  
  g.setColour(Colours::white);
  isButtonDown ? setAlpha(0.5f) : setAlpha(1.0f);
  
  if (isEnabled()) {
    g.drawRoundedRectangle(bounds.getX() + borderThick, bounds.getY() + borderThick,
                           bounds.getWidth() - 2*borderThick, bounds.getHeight() - 2*borderThick,
                           1, borderThick);
  }
  
  // TODO: write button text as grey if choice is completely unset?
  g.setFont(20);
  g.drawText(displayText, bounds.getX(), bounds.getY(),
             bounds.getWidth(), bounds.getHeight(),
             Justification::centred);
}

void SettingsCategoryButton::resized() {
  pillBounds.setSize(getLocalBounds().getWidth(), getLocalBounds().getHeight());
  fitRectInRect(pillBounds, getLocalBounds(), Justification::centred, false);
}

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

  double togWidth = h * 1.1f;
  double s = h * 0.25f;

  layout.setItemLayout(0, h, h, h);
  layout.setItemLayout(1, 0, s, s);
  layout.setItemLayout(2, togWidth, togWidth, togWidth);
  layout.setItemLayout(3, 0, s, s);
  layout.setItemLayout(4, 0, -1, -1);

  Component *comps[] = { icon, nullptr, toggle, nullptr, button };
  layout.layOutComponents(comps, 5, b.getX(), b.getY(), b.getWidth(), b.getHeight(), false, true);
}

void SettingsCategoryItemComponent::buttonClicked(Button *b) {
  if (b == toggle) {
    enabledStateChanged(toggle->getToggleState());
  }
}

void SettingsCategoryItemComponent::enablementChanged() {
  updateButtonText();
}

WifiCategoryItemComponent::WifiCategoryItemComponent() :
  SettingsCategoryItemComponent("wifi"),
  spinner(new WifiSpinner("SettingsWifiSpinner"))
{
  iconDrawable =
      Drawable::createFromImageFile(assetFile("wifiIcon.png"));
  icon->setImages(iconDrawable);
  bool isEnabled = getWifiStatus().isEnabled();
  toggle->setToggleState(isEnabled, NotificationType::dontSendNotification);
  button->setEnabled(isEnabled);
  addChildComponent(spinner);
  updateButtonText();
}

void WifiCategoryItemComponent::resized() {
  SettingsCategoryItemComponent::resized();
  const auto& sb = icon->getBoundsInParent();
  spinner->setBoundsToFit(sb.getX(), sb.getY(), sb.getWidth(), sb.getHeight(), Justification::centred, true);
}

void WifiCategoryItemComponent::enabledStateChanged(bool enabled) {
  updateButtonText();
  
  enabled ? getWifiStatus().setEnabled() : getWifiStatus().setDisabled();
}

void WifiCategoryItemComponent::handleWifiEnabled() {
  enableWifiActions();
}

void WifiCategoryItemComponent::handleWifiDisabled() {
  enableWifiActions();
}

void WifiCategoryItemComponent::handleWifiConnected() {
  enableWifiActions();
}

void WifiCategoryItemComponent::handleWifiDisconnected() {
  enableWifiActions();
}

void WifiCategoryItemComponent::handleWifiFailedConnect() {
  enableWifiActions();
}

void WifiCategoryItemComponent::handleWifiBusy() {
  disableWifiActions();
}

void WifiCategoryItemComponent::enableWifiActions() {
  spinner->hide();
  icon->setVisible(true);
  
  button->setEnabled(getWifiStatus().isEnabled());
  toggle->setEnabled(true);
  
  updateButtonText();
  toggle->setToggleState(getWifiStatus().isEnabled(), NotificationType::dontSendNotification);
}

void WifiCategoryItemComponent::disableWifiActions() {
  spinner->show();
  icon->setVisible(false);
  
  button->setEnabled(getWifiStatus().isEnabled());
  toggle->setEnabled(false);
  
  updateButtonText();
  toggle->setToggleState(getWifiStatus().isEnabled(), NotificationType::dontSendNotification);
}


void WifiCategoryItemComponent::updateButtonText() {
  const auto &status = getWifiStatus();
  if (status.isEnabled()) {
    if (status.isConnected() && status.connectedAccessPoint()) {
      button->setText(status.connectedAccessPoint()->ssid);
    }
    else {
      button->setText("Not Connected");
    }
  } else {
    button->setText("WiFi Off");
  }
}

BluetoothCategoryItemComponent::BluetoothCategoryItemComponent()
: SettingsCategoryItemComponent("bluetooth") {
  iconDrawable = Drawable::createFromImageFile(assetFile("bluetoothIcon.png"));
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
  bgImage = createImageFromFile(assetFile("settingsBackground.png"));
  mainPage = new Component();
  addAndMakeVisible(mainPage);
  mainPage->toBack();
  ChildProcess child{};

  brightness = 8;
  #if JUCE_LINUX
     // Get initial brightness value
     if(child.start("cat /sys/class/backlight/backlight/brightness")) {
    	String result{child.readAllProcessOutput()};
	brightness = result.getIntValue();
     };
  #endif


  volume = 90;
  
  #if JUCE_LINUX
    // Get initial volume value
    StringArray cmd{ "amixer","sget","Power Amplifier" };
    if(child.start(cmd)) {
      const String result (child.readAllProcessOutput());
      int resultIndex = result.indexOf("[")+1;
      child.waitForProcessToFinish (5 * 1000);
      char buff[4];
      for (int i = 0; i<4; i++) {
	      char c = result[resultIndex+i];
	      if( c >= '0' && c <= '9' ) {
		       buff[i]=c;
      	} else {
		     buff[i]=(char)0;
      	}
      }
      String newVol = String(buff);
      volume = newVol.getIntValue();
    }
  #endif

  ScopedPointer<Drawable> brightLo = Drawable::createFromImageFile(assetFile("brightnessIconLo.png"));
  ScopedPointer<Drawable> brightHi = Drawable::createFromImageFile(assetFile("brightnessIconHi.png"));
  screenBrightnessSlider =
      ScopedPointer<IconSliderComponent>(new IconSliderComponent(*brightLo, *brightHi));
  screenBrightnessSlider->addListener(this);
  screenBrightnessSlider->slider->setValue(1+(brightness-0.09)*10);

  ScopedPointer<Drawable> volLo =
      Drawable::createFromImageFile(assetFile("volumeIconLo.png"));
  ScopedPointer<Drawable> volHi =
      Drawable::createFromImageFile(assetFile("volumeIconHi.png"));
  volumeSlider = ScopedPointer<IconSliderComponent>(new IconSliderComponent(*volLo, *volHi));
  volumeSlider->addListener(this);
  volumeSlider->slider->setValue(volume);

  // create back button
  backButton = createImageButton(
                                 "Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  wifiCategoryItem = new WifiCategoryItemComponent();
  wifiCategoryItem->button->addListener(this);
  addAndMakeVisible(wifiCategoryItem);
  getWifiStatus().addListener(wifiCategoryItem);

  addAndMakeVisible(screenBrightnessSlider);
  addAndMakeVisible(volumeSlider);

  wifiPage = new SettingsPageWifiComponent();
}

SettingsPageComponent::~SettingsPageComponent() {}

void SettingsPageComponent::paint(Graphics &g) {
    auto bounds = getLocalBounds();
    g.fillAll(bgColor);
    g.drawImage(bgImage,bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, bgImage.getWidth(), bgImage.getHeight(), false);
}

void SettingsPageComponent::resized() {
  auto btnHeight = PokeLookAndFeel::getButtonHeight();
  auto bounds = getLocalBounds();
  int numRows = 3;
  double rowProp = 0.6/numRows;
  {
    for (int i = 0, j = 0; i < numRows; ++i) {
      if (i > 0) verticalLayout.setItemLayout(j++, 0, -1, -1);
      verticalLayout.setItemLayout(j++, -rowProp, -rowProp, -rowProp);
    }

    Component *settingsItems[] = {
      wifiCategoryItem, nullptr,
      screenBrightnessSlider, nullptr,
      volumeSlider
    };
    int numItems = sizeof(settingsItems) / sizeof(Component*);
    
    auto b = bounds;
    b.setLeft(btnHeight);
    b.setTop(btnHeight/2.f);
    b.setHeight(b.getHeight() - btnHeight/2.f);
    b.setWidth(b.getWidth() - btnHeight);
    verticalLayout.layOutComponents(settingsItems, numItems, b.getX(), b.getY(), b.getWidth(),
                                    b.getHeight(), true, true);
  }

  mainPage->setBounds(bounds);

  backButton->setBounds(bounds.getX(), bounds.getY(), btnHeight, bounds.getHeight());
}

void SettingsPageComponent::buttonClicked(Button *button) {
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
  } else if (button == wifiCategoryItem->button) {
    wifiPage->updateAccessPoints();
    getMainStack().pushPage(wifiPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}

void SettingsPageComponent::setSoundVolume() {
  volume = volumeSlider->slider->getValue();
  #if JUCE_LINUX
     StringArray cmd{ "amixer","sset","Power Amplifier",(String(volume)+"%").toRawUTF8()};
     if( child.start(cmd ) ) {
       String result{child.readAllProcessOutput()};
     }

  #endif
}

void SettingsPageComponent::setScreenBrightness() {
    brightness = 1+(screenBrightnessSlider->slider->getValue()*0.09);
    #if JUCE_LINUX
      StringArray cmd{ "sh","-c",(String("echo ") + String(brightness) + String(" > /sys/class/backlight/backlight/brightness")).toRawUTF8() };
      if( child.start(cmd) ) {
          String result{child.readAllProcessOutput()};
          DBG(result);
      }
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
