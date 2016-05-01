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

SettingsPageWifiComponent::SettingsPageWifiComponent() :
  nextPageBtn(createImageButton("NextAppsPage",
                                ImageFileFormat::loadFrom(assetFile("pageDownIcon.png")))),
  prevPageBtn(createImageButton("PrevAppsPage",
                                ImageFileFormat::loadFrom(assetFile("pageUpIcon.png"))))
{
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
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  // create wifi access point list "page"
  accessPointListPage = new Component("Access Point List Page");
  nextPageBtn->addListener(this);
  prevPageBtn->addListener(this);
  accessPointListPage->addAndMakeVisible(nextPageBtn);
  accessPointListPage->addAndMakeVisible(prevPageBtn);
  createAccessPointList();
  checkShowListNav();
  
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
  auto b = getLocalBounds();
  auto pb = Rectangle<int>(120, 0, b.getWidth() - 120, b.getHeight());

  pageStack->setBounds(pb);

  // FIXME: use scalable layout
  connectionLabel->setBounds(10, 50, pb.getWidth() - 20, 50);
  passwordEditor->setBounds(90, 100, pb.getWidth() - 180, 50);
  connectionButton->setBounds(90, 160, pb.getWidth() - 180, 50);
  errorLabel->setBounds(90, 210, pb.getWidth()-180, 50);
  wifiIconComponent->setBounds(10, 10, 60, 60);
  backButton->setBounds(b.getX(), b.getY(), 60, b.getHeight());
  const auto& cb = connectionButton->getLocalBounds();
  spinner->setBoundsToFit(cb.getX() + (cb.getHeight()/4.), cb.getY(), cb.getWidth(), cb.getHeight(), Justification::centredLeft, true);
  
  int btnHeight = 50;
  prevPageBtn->setSize(btnHeight, btnHeight);
  nextPageBtn->setSize(btnHeight, btnHeight);
  prevPageBtn->setBoundsToFit(0, 0, pb.getWidth(), pb.getHeight(), Justification::centredTop, true);
  nextPageBtn->setBoundsToFit(0, 0, pb.getWidth(), pb.getHeight(), Justification::centredBottom, true);
  // drop the page buttons from our available layout size
  auto trainWidth = pb.getWidth();
  auto trainHeight = pb.getHeight() - (2.0*btnHeight);
  accessPointList->setSize(trainWidth, trainHeight);
  accessPointList->setBoundsToFit(0, 0, pb.getWidth(), pb.getHeight(), Justification::centred, true);
  
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

void SettingsPageWifiComponent::handleWifiEnabled() {
  DBG("SettingsPageWifiComponent::wifiEnabled");
  spinner->hide();
}

void SettingsPageWifiComponent::handleWifiDisabled() {
  DBG("SettingsPageWifiComponent::wifiDisabled");
  spinner->hide();
  
  // if wifi is disabled while we're on this page, pop back to previous page.
  if (getMainStack().getCurrentPage() == this) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    // make sure we leave access list page as entry page.
    pageStack->clear(PageStackComponent::kTransitionNone);
    pageStack->pushPage(accessPointListPage, PageStackComponent::kTransitionNone);
  }
}

void SettingsPageWifiComponent::handleWifiConnected() {
  DBG("SettingsPageWifiComponent::wifiConnected");
  spinner->hide();

  selectedAp = getWifiStatus().connectedAccessPoint();

  updateConnectionLabel();
  passwordEditor->setVisible(false);
  connectionButton->setButtonText("Disconnect");
  errorLabel->setVisible(false);
  
  // if we're on the connection page, and we just connected, remove the
  // ability to go back to the access point list
  if (pageStack->getCurrentPage() == connectionPage
      && pageStack->getDepth() > 1) {
    pageStack->removePage(pageStack->getDepth() - 2);
  }
  // if we're on the access point list, and just connected, swap to the connection page
  else if (pageStack->getCurrentPage() == accessPointListPage) {
    // check if we're the current stack item, if we are our transition should be animated
    auto transition = (getMainStack().getCurrentPage() == this) ?
      PageStackComponent::kTransitionTranslateHorizontal : PageStackComponent::kTransitionNone;
    pageStack->swapPage(connectionPage, transition);
  }
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
  
  // add our access point list back into the stack, so it's available from back button
  if (pageStack->getCurrentPage() == connectionPage
      && pageStack->getDepth() == 1) {
    pageStack->insertPage(accessPointListPage, pageStack->getDepth() - 1);
  }
}

void SettingsPageWifiComponent::handleWifiBusy() {
  spinner->show();
}

void SettingsPageWifiComponent::beginSetConnected() {
  auto &status = getWifiStatus();
  
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
void SettingsPageWifiComponent::createAccessPointList() {
  // create ssid list
  accessPointList = new Grid(1, 4);
  
  accessPoints = getWifiStatus().nearbyAccessPoints();
  for (auto ap : accessPoints) {
    DBG(__func__ << ": added " << ap->ssid << ", " << ap->signalStrength << ", "
        << ap->requiresAuth);
    auto item = new WifiAccessPointListItem(ap, icons);
    item->addListener(this);
    accessPointItems.add(item);
    accessPointList->addItem(item);
  }
  
  accessPointListPage->addAndMakeVisible(accessPointList);
}


void SettingsPageWifiComponent::updateAccessPoints() {
  createAccessPointList();
  checkShowListNav();
  if (init) resized();
}

void SettingsPageWifiComponent::checkShowListNav() {
  if (accessPointList->hasNextPage()) {
    nextPageBtn->setVisible(true); nextPageBtn->setEnabled(true);
  }
  else {
    nextPageBtn->setVisible(false); nextPageBtn->setEnabled(false);
  }
  
  if (accessPointList->hasPrevPage()) {
    prevPageBtn->setVisible(true); prevPageBtn->setEnabled(true);
  }
  else {
    prevPageBtn->setVisible(false); prevPageBtn->setEnabled(false);
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
    else if (button == prevPageBtn) {
      accessPointList->showPrevPage();
      checkShowListNav();
    }
    else if (button == nextPageBtn) {
      accessPointList->showNextPage();
      checkShowListNav();
    }
    else if (button == backButton) {
      // leave connection page
      if (pageStack->getDepth() > 1) {
        pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
        checkShowListNav();
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

// FIXME: this is a hack for setting keyboard focus on animation completion
// why can't we set in when we queue the animation and have it respected on completion?
void SettingsPageWifiComponent::componentVisibilityChanged(Component& component) {
  // focus the password editor after the connection page finishes its transition in
  if (&component == connectionPage.get() && component.isVisible()) {
    passwordEditor->grabKeyboardFocus();
  }
}
