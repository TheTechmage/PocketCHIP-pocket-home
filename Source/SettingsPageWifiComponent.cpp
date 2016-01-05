#include "SettingsPageWifiComponent.h"
#include "Main.h"
#include "Utils.h"

WifiAccessPointListItem::WifiAccessPointListItem(const WifiAccessPoint &ap, WifiIcons *icons)
: Button{ ap.ssid }, ap(ap), icons{ icons } {}

void WifiAccessPointListItem::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bounds = getLocalBounds();
  auto w = bounds.getWidth(), h = bounds.getHeight();

  auto iconBounds = Rectangle<float>(w - h, 0, h, h);

  if (ap.requiresAuth) {
    icons->lockIcon->drawWithin(g, iconBounds, RectanglePlacement::fillDestination, 1.0f);
  }

  iconBounds.translate(-h, 0);

  icons->wifiStrength[ap.signalStrength]->drawWithin(g, iconBounds,
                                                     RectanglePlacement::fillDestination, 1.0f);

  g.setFont(Font(getLookAndFeel().getTypefaceForFont(Font())));
  g.setFont(h);
  g.setColour(findColour(DrawableButton::textColourId));
  g.drawText(getName(), bounds, Justification::centredLeft);
}

SettingsPageWifiComponent::SettingsPageWifiComponent() {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  wifiIconComponent = new ImageComponent("WiFi Icon");
  wifiIconComponent->setImage(
      ImageFileFormat::loadFrom(BinaryData::wifiIcon_png, BinaryData::wifiIcon_pngSize));
  addAndMakeVisible(wifiIconComponent);

  icons = new WifiIcons();

  icons->lockIcon = Drawable::createFromImageData(BinaryData::lock_png, BinaryData::lock_pngSize);

  icons->wifiStrength = OwnedArray<Drawable>();
  icons->wifiStrength.set(0, Drawable::createFromImageData(BinaryData::wifiStrength0_png,
                                                           BinaryData::wifiStrength0_pngSize));
  icons->wifiStrength.set(1, Drawable::createFromImageData(BinaryData::wifiStrength1_png,
                                                           BinaryData::wifiStrength1_pngSize));
  icons->wifiStrength.set(2, Drawable::createFromImageData(BinaryData::wifiStrength2_png,
                                                           BinaryData::wifiStrength2_pngSize));
  icons->wifiStrength.set(3, Drawable::createFromImageData(BinaryData::wifiStrength3_png,
                                                           BinaryData::wifiStrength3_pngSize));

  // create back button
  ScopedPointer<Drawable> backButtonDrawable =
      Drawable::createFromImageData(BinaryData::backIcon_png, BinaryData::backIcon_pngSize);
  backButton = createImageButtonFromDrawable("Back", *backButtonDrawable);
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  // create ssid list "page"
  accessPointListPage = new TrainComponent();
  accessPointListPage->setOrientation(TrainComponent::kOrientationVertical);
  accessPointListPage->itemHeight = 32;
  accessPointListPage->itemScaleMin = 0.9f;

  auto wifiListJson = parseWifiListJson("../../assets/wifi.json");
  for (const auto &apJson : *wifiListJson.getArray()) {
    WifiAccessPoint ap;
    ap.ssid = apJson["name"];
    ap.signalStrength = apJson["strength"];
    ap.requiresAuth = apJson["auth"];
    auto item = new WifiAccessPointListItem(ap, icons);
    item->addListener(this);
    accessPointItems.add(item);
    accessPointListPage->addItem(item);
  }

  // create connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  passwordEditor = new TextEditor("Password", (juce_wchar)0x2022);
  passwordEditor->setText("Password");
  connectionPage->addAndMakeVisible(passwordEditor);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->addListener(this);
  connectionPage->addAndMakeVisible(connectionButton);
}

SettingsPageWifiComponent::~SettingsPageWifiComponent() {}

void SettingsPageWifiComponent::paint(Graphics &g) {}

void SettingsPageWifiComponent::setWifiEnabled(bool enabled) {
  pageStack->setVisible(enabled);
  if (enabled && pageStack->getCurrentPage() != connectionPage) {
    pageStack->clear(PageStackComponent::kTransitionNone);
    Component *nextPage = wifiConnected ? connectionPage : accessPointListPage;
    pageStack->pushPage(nextPage, PageStackComponent::kTransitionNone);
  }
}

var SettingsPageWifiComponent::parseWifiListJson(const String &path) {
  auto ssidListFile = absoluteFileFromPath(path);
  auto ssidListJson = JSON::parse(ssidListFile);
  if (!ssidListJson) {
    std::cerr << "Could not read wifi.json file from " << ssidListFile.getFullPathName()
              << std::endl;
  }
  return ssidListJson;
}

void SettingsPageWifiComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  backButton->setBounds(10, 10, 62, 62);

  pageStack->setBounds(pageBounds);

  connectionLabel->setBounds(10, 90, pageBounds.getWidth() - 20, 24);
  passwordEditor->setBounds(90, 120, pageBounds.getWidth() - 180, 24);
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 24);

  wifiIconComponent->setBounds(bounds.getX() + 7, bounds.getHeight() / 2.0f - 40, 80, 80);

  if (!init) { // TODO: cruft to resize page correctly on init? arrg. Should be in constructor,
               //  or not at all
    init = true;
    pageStack->pushPage(accessPointListPage, PageStackComponent::kTransitionNone);
  }
}

void SettingsPageWifiComponent::buttonClicked(Button *button) {
  passwordEditor->setVisible(false);

  if (button == connectionButton) {
    if (wifiConnected && selectedAp == connectedAp) {
      connectionButton->setButtonText("Connect");
      passwordEditor->setVisible(connectedAp->requiresAuth);
      wifiConnected = false;
      connectedAp = nullptr;
      pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
    } else {
      connectionButton->setButtonText("Disconnect");
      wifiConnected = true;
      connectedAp = selectedAp;
    }
  }

  auto apButton = dynamic_cast<WifiAccessPointListItem *>(button);
  if (apButton) {
    selectedAp = &apButton->ap;
    connectionLabel->setText(apButton->ap.ssid, juce::NotificationType::dontSendNotification);
    if (selectedAp == connectedAp) {
      connectionButton->setButtonText("Disconnect");
    } else {
      passwordEditor->setVisible(apButton->ap.requiresAuth);
      connectionButton->setButtonText("Connect");
    }
    pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }

  if (button == backButton) {
    if (pageStack->getDepth() > 1) {
      pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
    } else {
      getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    }
  }
}
