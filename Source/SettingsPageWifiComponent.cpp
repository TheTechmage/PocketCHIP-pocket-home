#include "SettingsPageWifiComponent.h"
#include "PokeLookAndFeel.h"
#include "Main.h"
#include "Utils.h"

WifiSpinner::WifiSpinner(const String& componentName) :
  ImageComponent(componentName)
{
  const Array<String> spinnerImgPaths{
    "spinner0.png","spinner1.png","spinner2.png","spinner3.png",
    "spinner4.png","spinner5.png","spinner6.png","spinner7.png"};

  for(auto& path : spinnerImgPaths) {
    auto image = createImageFromFile(assetFile(path));
    images.add(image);
  }
  
  const auto& startImg = images[0];
  setImage(startImg);
  setSize(startImg.getWidth(), startImg.getHeight());
  
  timer.spinner = this;
}

WifiSpinner::~WifiSpinner() {
  timer.stopTimer();
  timer.spinner = nullptr;
}

void WifiSpinner::hide() {
  setVisible(false);
  timer.stopTimer();
}

void WifiSpinner::show() {
  setVisible(true);
  timer.startTimer(500);
}

void WifiSpinner::nextImage() {
  i++;
  if (i == images.size()) { i = 0; }
  setImage(images[i]);
}

void WifiSpinnerTimer::timerCallback() {
  spinner->nextImage();
}

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

  if (!ap) {
     DBG(__func__ << ": ERROR: trying to paint NULL AP!!!!");
     return;
  }
  icons->wifiStrength[wifiSignalStrengthToIdx(ap->signalStrength)]->drawWithin(g, iconBounds,
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

int WifiAccessPointListItem::wifiSignalStrengthToIdx(int strength) {
    // 0 to 100
    float sigStrength = std::max(0., std::fmin(100, strength));
    int iconBins = icons->wifiStrength.size() - 1;
    return round( ( iconBins * (sigStrength)/100.0f) );
}

SettingsPageWifiComponent::SettingsPageWifiComponent() {
  bgColor = Colour(PokeLookAndFeel::chipPurple);
  bgImage = createImageFromFile(assetFile("settingsBackground.png"));
  
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  wifiIconComponent = new ImageComponent("WiFi Icon");
  wifiIconComponent->setImage(
      ImageFileFormat::loadFrom(assetFile("wifiStrength3.png")));
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

  updateAccessPoints();
  
  // create connection "page"
  connectionPage = new Component("Connection Page");
  connectionPage->addComponentListener(this);

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setFont(26);
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  passwordEditor = new TextEditor("Password", (juce_wchar)0x2022);
  passwordEditor->setFont(26);
  passwordEditor->addListener(this);
  connectionPage->addAndMakeVisible(passwordEditor);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->addListener(this);
  connectionButton->setTriggeredOnMouseDown(true);
  connectionPage->addAndMakeVisible(connectionButton);
    
  errorLabel = new Label("Error Text", "Bad password ...");
  errorLabel->setFont(26);
  errorLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addChildComponent(errorLabel);
  
  // add the spinner image to our page
  spinner = new WifiSpinner("WifiSpinner");
  connectionButton->addChildComponent(spinner);
  
  // register for wifi status events
  getWifiStatus().addListener(this);
}

SettingsPageWifiComponent::~SettingsPageWifiComponent() {}

void SettingsPageWifiComponent::paint(Graphics &g) {
    auto bounds = getLocalBounds();
    g.fillAll(bgColor);
    g.drawImage(bgImage,bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, bgImage.getWidth(), bgImage.getHeight(), false);
}

void SettingsPageWifiComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  pageStack->setBounds(pageBounds);

  // FIXME: use scalable layout
  connectionLabel->setBounds(10, 50, pageBounds.getWidth() - 20, 50);
  passwordEditor->setBounds(90, 100, pageBounds.getWidth() - 180, 50);
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 50);
  errorLabel->setBounds(90, 210, pageBounds.getWidth()-180, 50);
  wifiIconComponent->setBounds(10, 10, 60, 60);
  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());
  const auto& cb = connectionButton->getLocalBounds();
  spinner->setBoundsToFit(cb.getX() + 6, cb.getY(), cb.getWidth(), cb.getHeight(), Justification::centredLeft, true);
  
  // FIXME: this logic belongs in constructor, but sizing info shows wrong on resize.
  if (!init) {
    init = true;

    // check wifi status to pick correct initial page
    auto& wifiStatus = getWifiStatus();
    if (wifiStatus.isConnected()) {
      selectedAp = wifiStatus.connectedAccessPoint();
      updateConnectionLabel();
      passwordEditor->setVisible(false);
      connectionButton->setButtonText("Disconnect");
      pageStack->pushPage(connectionPage, PageStackComponent::kTransitionNone);
    }
    else {
      pageStack->pushPage(accessPointListPage, PageStackComponent::kTransitionNone);
    }
  }
}

void SettingsPageWifiComponent::handleWifiDisabled() {
  DBG("SettingsPageWifiComponent::wifiDisabled");
  // TODO: this event should probably kick you out of this page entirely
  // though how did you get here (parent should block entry)? should only deliver here if we're on screen
  // and WiFi dies out of band
}

// FIXME: these handlers should double check what page we're on,
// in case these occur transiently. We don't want global page stack popping.
// Really there should be a lightweight statemachine somewhere around this class.
void SettingsPageWifiComponent::handleWifiConnected() {
  DBG("SettingsPageWifiComponent::wifiConnected");
  
  spinner->hide();
  
  updateConnectionLabel();
  passwordEditor->setVisible(false);
  connectionButton->setButtonText("Disconnect");
  errorLabel->setVisible(false);
  pageStack->removePage(pageStack->getDepth() - 2);
}

void SettingsPageWifiComponent::handleWifiFailedConnect() {
  DBG("SettingsPageWifiComponent::wifiFailedConnect");
  
  spinner->hide();
  updateConnectionLabel();
  
  if (selectedAp->requiresAuth) {
    errorLabel->setVisible(true);
    passwordEditor->setText("");
  }
}

void SettingsPageWifiComponent::handleWifiDisconnected() {
  DBG("SettingsPageWifiComponent::wifiDisconnected");
  
  spinner->hide();
  updateConnectionLabel();
  
  if (selectedAp->requiresAuth) {
    passwordEditor->setVisible(true);
  }
  connectionButton->setButtonText("Connect");
  errorLabel->setVisible(false);
  
  updateAccessPoints();
  
  pageStack->insertPage(accessPointListPage, pageStack->getDepth() - 1);
}

void SettingsPageWifiComponent::beginSetConnected() {
  auto &status = getWifiStatus();
  
  spinner->show();
  errorLabel->setVisible(false);
  
  if (selectedAp->requiresAuth) {
    const auto& psk = passwordEditor->getTextValue().toString();
    status.setConnectedAccessPoint(selectedAp, psk);
  }
  else {
    status.setConnectedAccessPoint(selectedAp);
  }
}

void SettingsPageWifiComponent::beginSetDisconnected() {
  spinner->show();
  getWifiStatus().setDisconnected();
}

void SettingsPageWifiComponent::updateConnectionLabel() {
  String ssidText = selectedAp->ssid;
  const auto& status = getWifiStatus();
  
  if (status.isConnected() &&
      status.connectedAccessPoint()->hash == selectedAp->hash) {
    ssidText += " (connected)";
  }
  
  connectionLabel->setText(ssidText, juce::NotificationType::dontSendNotification);
}

// TODO: this is pretty expensive, but the cleanup is very simple. Could be replaced with a change
// listener, or a merge operation.
void SettingsPageWifiComponent::updateAccessPoints() {
  // create ssid list
  accessPointListPage = new TrainComponent(TrainComponent::kOrientationVertical);
  accessPointListPage->itemHeight = 50;
  accessPointListPage->itemScaleMin = accessPointListPage->itemScaleMax = 1.0;
  
  accessPoints = getWifiStatus().nearbyAccessPoints();
  for (auto ap : accessPoints) {
    DBG(__func__ << ": added " << ap->ssid << ", " << ap->signalStrength << ", "
        << ap->requiresAuth);
    auto item = new WifiAccessPointListItem(ap, icons);
    item->addListener(this);
    accessPointItems.add(item);
    accessPointListPage->addItem(item);
  }
}

void SettingsPageWifiComponent::buttonClicked(Button *button) {
  auto &status = getWifiStatus();
  
  // button from the connection dialog
  if (button == connectionButton) {
    if (status.isConnected()) {
      beginSetDisconnected();
    } else {
      beginSetConnected();
    }
  }
  // button from the ap list
  else {
    auto apButton = dynamic_cast<WifiAccessPointListItem *>(button);
    if (apButton) {
      selectedAp = new WifiAccessPoint(*apButton->ap);
      updateConnectionLabel();
      if (status.isConnected() &&
          selectedAp->hash == status.connectedAccessPoint()->hash) {
        passwordEditor->setText(String::empty);
        passwordEditor->setVisible(false);
        errorLabel->setVisible(false);
        connectionButton->setButtonText("Disconnect");
      } else {
        passwordEditor->setText(String::empty);
        passwordEditor->setVisible(apButton->ap->requiresAuth);
        errorLabel->setVisible(false);
        connectionButton->setButtonText("Connect");
      }
      pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
    }
    
    if (button == backButton) {
      // leave connection page
      if (pageStack->getDepth() > 1) {
        pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
      // leave wifi settings page
      } else {
        getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
      }
    }
  }
}

void SettingsPageWifiComponent::textEditorReturnKeyPressed(TextEditor &) {
  beginSetConnected();
}

void SettingsPageWifiComponent::componentVisibilityChanged(Component& component) {
  // focus the password editor after the connection page finishes its transition in
  if (&component == connectionPage.get() && component.isVisible()) {
    passwordEditor->grabKeyboardFocus();
  }
}
