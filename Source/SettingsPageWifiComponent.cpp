#include "SettingsPageWifiComponent.h"
#include "PokeLookAndFeel.h"
#include "Main.h"
#include "Utils.h"

WifiAccessPointListItem::WifiAccessPointListItem(WifiAccessPoint *ap, WifiIcons *icons)
: Button{ ap->ssid }, ap{ ap }, icons{ icons } {}

void WifiAccessPointListItem::resized() {
  setSize(getLocalBounds().getWidth(), 42);
}

void WifiAccessPointListItem::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bounds = getLocalBounds();
  auto inset = bounds.reduced(6, 4);
  auto w = bounds.getWidth(), h = bounds.getHeight();
  auto iconBounds = Rectangle<float>(w - h, h/5.0, h*0.6, h*0.6);
  auto borderThick = 4.0;
  float radius = float(bounds.getHeight()) / 2.0f;

  g.setColour(findColour(ListBox::ColourIds::backgroundColourId));
  g.drawRoundedRectangle(bounds.getX() + borderThick, bounds.getY() + borderThick,
                         bounds.getWidth() - 2*borderThick, bounds.getHeight()  - 2*borderThick,
                         radius, borderThick);

  icons->wifiStrength[ap->signalStrength]->drawWithin(g, iconBounds,
                                                      RectanglePlacement::fillDestination, 1.0f);
  if (ap->requiresAuth) {
    iconBounds.translate(-h * 0.75, 0);
    icons->lockIcon->drawWithin(g, iconBounds, RectanglePlacement::fillDestination, 1.0f);
  }

  g.setFont(Font(getLookAndFeel().getTypefaceForFont(Font())));
  g.setFont(h * 0.5);
  g.setColour(findColour(ListBox::ColourIds::textColourId));
  g.drawText(getName(), inset.reduced(h * 0.3, 0), Justification::centredLeft);
}

SettingsPageWifiComponent::SettingsPageWifiComponent() {
  bgColor = Colour(PokeLookAndFeel::chipPurple);
  bgImage = String("settingsBackground.png");
  
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  wifiIconComponent = new ImageComponent("WiFi Icon");
  wifiIconComponent->setImage(
      ImageFileFormat::loadFrom(assetFile("wifiStrength3,png")));
  addAndMakeVisible(wifiIconComponent);

  icons = new WifiIcons();

  icons->lockIcon = Drawable::createFromImageFile(assetFile("lock.png"));

  icons->wifiStrength = OwnedArray<Drawable>();
  icons->wifiStrength.set(0, Drawable::createFromImageFile(assetFile("wifiStrength0.png")));
  icons->wifiStrength.set(1, Drawable::createFromImageFile(assetFile("wifiStrength1.png")));
  icons->wifiStrength.set(2, Drawable::createFromImageFile(assetFile("wifiStrength2.png")));
  icons->wifiStrength.set(3, Drawable::createFromImageFile(assetFile("wifiStrength3.png")));

  icons->arrowIcon = Drawable::createFromImageFile(assetFile("backIcon.png"));
  auto xf = AffineTransform::identity.rotated(M_PI);
  icons->arrowIcon->setTransform(xf);

  // create back button
  backButton = createImageButton(
                                 "Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
   backButton->setTriggeredOnMouseDown(true);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  // create ssid list
  accessPointListPage = new TrainComponent(TrainComponent::kOrientationVertical);
  accessPointListPage->itemHeight = 50;
  accessPointListPage->itemScaleMin = accessPointListPage->itemScaleMax = 1.0;

  for (auto ap : *getWifiStatus().nearbyAccessPoints()) {
    auto item = new WifiAccessPointListItem(ap, icons);
    item->addListener(this);
    accessPointItems.add(item);
    accessPointListPage->addItem(item);
  }
  
  // register for wifi status events
  getWifiStatus().addListener(this);

  // create connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setFont(26);
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  passwordEditor = new TextEditor("Password", (juce_wchar)0x2022);
  passwordEditor->setFont(26);
  passwordEditor->setTextToShowWhenEmpty("password", findColour(TextEditor::ColourIds::textColourId));
  connectionPage->addAndMakeVisible(passwordEditor);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->addListener(this);
  connectionButton->setTriggeredOnMouseDown(true);
  connectionPage->addAndMakeVisible(connectionButton);
}

SettingsPageWifiComponent::~SettingsPageWifiComponent() {}

void SettingsPageWifiComponent::paint(Graphics &g) {
    g.fillAll(bgColor);
    //auto image = createImageFromFile(assetFile(bgImage));
    //g.drawImageAt(image,0,0,false);
}

void SettingsPageWifiComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  pageStack->setBounds(pageBounds);

  connectionLabel->setBounds(10, 50, pageBounds.getWidth() - 20, 50);
  passwordEditor->setBounds(90, 100, pageBounds.getWidth() - 180, 50);
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 50);
  wifiIconComponent->setBounds(10, 10, 60, 60);
  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());

  if (!init) { // TODO: cruft to resize page correctly on init? arrg. Should be in constructor,
               //  or not at all
    init = true;
    pageStack->pushPage(accessPointListPage, PageStackComponent::kTransitionNone);
  }
}

void SettingsPageWifiComponent::handleWifiDisabled() {
  DBG("SettingsPageWifiComponent::disable");
  // TODO: this event should probably kick you out of this page entirely
  // though how did you get here (parent should block entry)? should only deliver here if we're on screen
  // and WiFi dies out of band
}

// FIXME: these handlers should double check what page we're on,
// in case these occur transiently. We don't want global page stack popping.
// Really there should be a lightweight statemachine somewhere around this class.
void SettingsPageWifiComponent::handleWifiConnected() {
  DBG("SettingsPageWifiComponent::connect");
  getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
  passwordEditor->setVisible(false);
  connectionButton->setButtonText("Disconnect");
}

void SettingsPageWifiComponent::handleWifiFailedConnect() {
  passwordEditor->setText("");
}

void SettingsPageWifiComponent::handleWifiDisconnected() {
  DBG("SettingsPageWifiComponent::disconnect");
  connectionButton->setButtonText("Connect");
  pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
}

void SettingsPageWifiComponent::buttonClicked(Button *button) {
  auto &status = getWifiStatus();

  if (button == connectionButton) {
    if (status.isConnected() && selectedAp == status.connectedAccessPoint()) {
      status.setDisconnected();
    } else {
      if (selectedAp->requiresAuth) {
        const auto& psk = passwordEditor->getTextValue().toString();
        status.setConnectedAccessPoint(selectedAp, psk);
      }
      else {
        status.setConnectedAccessPoint(selectedAp);
      }
    }
  }
  else {
    auto apButton = dynamic_cast<WifiAccessPointListItem *>(button);
    if (apButton) {
      selectedAp = apButton->ap;
      connectionLabel->setText(apButton->ap->ssid, juce::NotificationType::dontSendNotification);
      if (selectedAp == status.connectedAccessPoint()) {
        passwordEditor->setVisible(false);
        connectionButton->setButtonText("Disconnect");
      } else {
        passwordEditor->setVisible(apButton->ap->requiresAuth);
        connectionButton->setButtonText("Connect");
      }
      pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
    }
    
    if (button == backButton) {
      if (pageStack->getDepth() > 1 && !status.isConnected()) {
        pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
      } else {
        getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
      }
    }
  }
}
