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

void WifiAccessPointListItem::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bounds = getLocalBounds();
  auto inset = bounds.reduced(bounds.getWidth() / 10, bounds.getHeight() / 10);
  auto w = bounds.getWidth(), h = bounds.getHeight();
  auto iconBounds = Rectangle<float>(w - h, h/5.0, h*0.5, h*0.5);
  auto borderThick = jmax(1, jmin(bounds.getWidth() / 9, bounds.getHeight() / 9));

  g.setColour(findColour(ListBox::ColourIds::backgroundColourId));
  isButtonDown ? setAlpha(0.5f) : setAlpha(1.0f);
  g.drawRoundedRectangle(bounds.getX() + borderThick, bounds.getY() + borderThick,
                         bounds.getWidth() - 4*borderThick, bounds.getHeight()  - 2*borderThick,
                         1, borderThick);

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
      ImageFileFormat::loadFrom(assetFile("wifiIcon.png")));
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
  
  int btnH = PokeLookAndFeel::getButtonHeight();
  auto pb = Rectangle<int>(btnH, 0, b.getWidth() - btnH*2, b.getHeight());
  
  // top level page elements
  pageStack->setBounds(pb);
  wifiIconComponent->setBounds(b.getX() + btnH/10, b.getY() + btnH/12, btnH * 0.9f, btnH * 0.9f);
  backButton->setBounds(b.getX(), b.getY(), btnH, b.getHeight());
  
  // individual connection page
  connectionLabel->setBounds(0, pb.getY(), pb.getWidth(), btnH);
  passwordEditor->setBounds(btnH, pb.getY(), pb.getWidth() - btnH*2, btnH);
  connectionButton->setBounds(btnH, pb.getY(), pb.getWidth() - btnH*2, btnH);
  errorLabel->setBounds(btnH, pb.getY(), pb.getWidth() - btnH*2, btnH);
  
  StretchableLayoutManager connectionLayout{};
  Component* connectionItems[] = {nullptr, connectionLabel, passwordEditor, connectionButton, errorLabel, nullptr};
  connectionLayout.setItemLayout(0, -0, -1, -1);
  connectionLayout.setItemLayout(1, btnH, btnH, btnH);
  connectionLayout.setItemLayout(2, btnH, btnH, btnH);
  connectionLayout.setItemLayout(3, btnH, btnH, btnH);
  connectionLayout.setItemLayout(4, btnH, btnH, btnH);
  connectionLayout.setItemLayout(5, -0, -1, -1);
  connectionLayout.layOutComponents(connectionItems, 6, pb.getX(), pb.getY(), pb.getWidth(), pb.getHeight(), true, false);
  
  const auto& cb = connectionButton->getLocalBounds();
  spinner->setBoundsToFit(cb.getX(), cb.getY(), cb.getWidth(), cb.getHeight(), Justification::centred, true);
  
  // wifi ap list
  prevPageBtn->setSize(btnH, btnH);
  nextPageBtn->setSize(btnH, btnH);
  prevPageBtn->setBoundsToFit(0, 0, pb.getWidth(), pb.getHeight(), Justification::centredTop, true);
  nextPageBtn->setBoundsToFit(0, 0, pb.getWidth(), pb.getHeight(), Justification::centredBottom, true);
  // drop the page buttons from our available layout size
  auto trainWidth = pb.getWidth();
  auto trainHeight = pb.getHeight() - btnH*2;
  accessPointList->setSize(trainWidth, trainHeight);
  accessPointList->setBoundsToFit(0, 0, pb.getWidth(), pb.getHeight(), Justification::centred, true);
  
  // FIXME: this logic belongs in constructor, but sizing info shows wrong on resize.
  if (!init) {
    init = true;

    // check wifi status to pick correct initial page
    auto& wifiStatus = getWifiStatus();
    if (wifiStatus.isConnected()) {
      selectedAp = wifiStatus.connectedAccessPoint();
      updateConnectionLabelAndButton();
      passwordEditor->setVisible(false);
      pageStack->pushPage(connectionPage, PageStackComponent::kTransitionNone);
    }
    else {
      pageStack->pushPage(accessPointListPage, PageStackComponent::kTransitionNone);
    }
  }
}

void SettingsPageWifiComponent::handleWifiEnabled() {
  DBG("SettingsPageWifiComponent::wifiEnabled");
  
  enableWifiActions();
}

void SettingsPageWifiComponent::handleWifiDisabled() {
  DBG("SettingsPageWifiComponent::wifiDisabled");
  
  enableWifiActions();
  
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
  selectedAp = getWifiStatus().connectedAccessPoint();

  enableWifiActions();
  passwordEditor->setVisible(false);
  errorLabel->setVisible(false);
  
  // if we're on the connection page
  if (pageStack->getCurrentPage() == connectionPage) {
    // remove the ability to go back to the access point list
    if (pageStack->getDepth() > 1) {
      pageStack->removePage(pageStack->getDepth() - 2);
    }
  }
  // if we're on the access point list
  else if (pageStack->getCurrentPage() == accessPointListPage) {
    // swap to the connection page
    pageStack->swapPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
  
  // pop back to previous page if we're focused
  if (getMainStack().getCurrentPage() == this)
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
}

void SettingsPageWifiComponent::handleWifiFailedConnect() {
  DBG("SettingsPageWifiComponent::wifiFailedConnect");
  
  enableWifiActions();
  
  if (selectedAp && selectedAp->requiresAuth) {
    errorLabel->setVisible(true);
    passwordEditor->setText("");
    passwordEditor->grabKeyboardFocus();
  }
  
  // make sure we show the ap list from back button
  // add our access point list back into the stack, so it's available from back button
  if (pageStack->getDepth() == 1) {
    pageStack->insertPage(accessPointListPage, 0);
  }
}

void SettingsPageWifiComponent::handleWifiDisconnected() {
  DBG("SettingsPageWifiComponent::wifiDisconnected");
  
  enableWifiActions();
  updateAccessPoints();
  
  if (selectedAp && selectedAp->requiresAuth) {
    passwordEditor->setVisible(true);
    passwordEditor->grabKeyboardFocus();
  }
  errorLabel->setVisible(false);
  
  // if we receive disconnect while on connection page
  if (pageStack->getCurrentPage() == connectionPage) {
    // add our access point list back into the stack, so it's available from back button
    if (pageStack->getDepth() == 1) {
      pageStack->insertPage(accessPointListPage, 0);
    }
    
    // go back to ap list
    pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
  }
}

void SettingsPageWifiComponent::handleWifiBusy() {
  disableWifiActions();
}

void SettingsPageWifiComponent::enableWifiActions() {
  bool isEnabled = getWifiStatus().isEnabled();
  
  spinner->hide();
  connectionButton->setEnabled(isEnabled);
  passwordEditor->setEnabled(isEnabled);
  
  updateConnectionLabelAndButton();
}

void SettingsPageWifiComponent::disableWifiActions() {
  spinner->show();
  connectionButton->setButtonText("");

  connectionButton->setEnabled(false);
  passwordEditor->setEnabled(false);
}

void SettingsPageWifiComponent::beginSetConnected() {
  auto &status = getWifiStatus();
  
  errorLabel->setVisible(false);
  
  // make sure we hide the ap list while attempting to connect
  if (pageStack->getDepth() > 1) {
    pageStack->removePage(pageStack->getDepth() - 2);
  }
  
  if (selectedAp && selectedAp->requiresAuth) {
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

void SettingsPageWifiComponent::updateConnectionLabelAndButton() {
  const auto& status = getWifiStatus();
  String ssidText;
  String buttonText;
  
  if (selectedAp) {
    ssidText = selectedAp->ssid;
    buttonText = "Connect";
    
    auto connectedAp = status.connectedAccessPoint();
    if (status.isConnected() && connectedAp &&
        connectedAp->hash == selectedAp->hash) {
      ssidText += " (connected)";
      buttonText = "Disconnect";
    }
  }
  else {
    ssidText = "Access point disconnected.";
    buttonText = "...";
  }
  
  connectionLabel->setText(ssidText, juce::NotificationType::dontSendNotification);
  connectionButton->setButtonText(buttonText);
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

void SettingsPageWifiComponent::buttonStateChanged(Button *btn) {
  if (btn->isMouseButtonDown() && btn->isMouseOver()) {
    btn->setAlpha(0.5f);
  }
  else {
    btn->setAlpha(1.0f);
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
      auto connectedAp = status.connectedAccessPoint().get();
      
      updateConnectionLabelAndButton();
      
      if (status.isConnected() && connectedAp &&
          connectedAp->hash == selectedAp->hash) {
        passwordEditor->setText(String::empty);
        passwordEditor->setVisible(false);
        errorLabel->setVisible(false);
      } else {
        passwordEditor->setText(String::empty);
        passwordEditor->setVisible(apButton->ap->requiresAuth);
        errorLabel->setVisible(false);
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
