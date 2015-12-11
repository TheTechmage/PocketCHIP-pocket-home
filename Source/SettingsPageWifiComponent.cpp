#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageWifiComponent.h"
#include "Main.h"
#include "Utils.h"

SettingsPageWifiComponent::SettingsPageWifiComponent() {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  ScopedPointer<XmlElement> wifiSvg = XmlDocument::parse(BinaryData::wifiIcon_svg);
  wifiIcon = Drawable::createFromSVG(*wifiSvg);
  addAndMakeVisible(wifiIcon);

  switchComponent = new SwitchComponent();
  switchComponent->addListener(this);
  switchComponent->toFront(false);
  addAndMakeVisible(switchComponent);
  
  // create back button
  ScopedPointer<XmlElement> backButtonSvg = XmlDocument::parse(BinaryData::backIcon_svg);
  ScopedPointer<Drawable> backButtonDrawable = Drawable::createFromSVG(*backButtonSvg);  
  backButton = createImageButtonFromDrawable("Back", *backButtonDrawable);
  backButton->addListener(this);  
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  // create ssid list "page"
  ssidListPage = new Component("SSID List Page");

  ssidList = new ListBox();
  ssidList->setModel(this);
  ssidList->setMultipleSelectionEnabled(false);
  ssidListPage->addAndMakeVisible(ssidList);

  // create connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  passwordEditor = new TextEditor("Password", (juce_wchar)0x2022);
  passwordEditor->setText("Password");
  passwordEditor->setColour(TextEditor::ColourIds::backgroundColourId, Colours::lightgrey);
  connectionPage->addAndMakeVisible(passwordEditor);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->addListener(this);
  connectionPage->addAndMakeVisible(connectionButton);

  // add pages to page stack
  pageStack->addChildComponent(ssidListPage);
  pageStack->addChildComponent(connectionPage);
}

SettingsPageWifiComponent::~SettingsPageWifiComponent() {}

void SettingsPageWifiComponent::setWifiEnabled(Boolean enabled) {
  pageStack->setVisible(enabled);
  if (enabled) {
    auto nextPage = wifiConnected ? &connectionPage : &ssidListPage;
    pageStack->pushPage(*nextPage, PageStackComponent::kTransitionNone);
  }
}

void SettingsPageWifiComponent::paint(Graphics &g) {}

void SettingsPageWifiComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  backButton->setBounds(10, 10, 62, 62);

  pageStack->setBounds(pageBounds);
  ssidList->setBounds(0, 0, pageBounds.getWidth(), pageBounds.getHeight());

  connectionLabel->setBounds(10, 90, pageBounds.getWidth() - 20, 24);
  passwordEditor->setBounds(90, 120, pageBounds.getWidth() - 180, 24);
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 24);

  wifiIcon->setTopLeftPosition(bounds.getX(), bounds.getHeight() / 2.0f - 20);

  {
    auto t = switchComponent->getTransform();
    t = AffineTransform::identity.rotated(-float_Pi / 2.0)
            .translated(bounds.getX() + 75, bounds.getHeight() / 2.0f + 40);
    switchComponent->setTransform(t);
  }
}

void SettingsPageWifiComponent::buttonClicked(Button *button) {
  if (button == connectionButton) {
    if (wifiConnected) {
      passwordEditor->setVisible(true);
      connectionButton->setButtonText("Connect");
      wifiConnected = false;
      pageStack->pushPage(ssidListPage, PageStackComponent::kTransitionNone);
    } else {
      passwordEditor->setVisible(false);
      connectionButton->setButtonText("Disconnect");
      wifiConnected = true;
    }
  }
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
  }  
}

void SettingsPageWifiComponent::buttonStateChanged(Button *button) {
  if (button == switchComponent && wifiEnabled != button->getToggleState()) {
    wifiEnabled = button->getToggleState();
    setWifiEnabled(wifiEnabled);
  }
}

int SettingsPageWifiComponent::getNumRows() {
  return 15;
}

void SettingsPageWifiComponent::paintListBoxItem(int rowNumber, Graphics &g, int width, int height,
                                                 bool rowIsSelected) {
  if (rowIsSelected) g.fillAll(Colours::lightgrey);
  g.setColour(Colours::black);
  g.setFont(height * 0.7f);
  g.drawText("WIFI SSID " + String(rowNumber), 5, 0, width, height, Justification::centredLeft,
             true);
}

void SettingsPageWifiComponent::listBoxItemClicked(int rowNumber, const MouseEvent &) {
  connectionLabel->setText("WIFI SSID " + String(rowNumber),
                           juce::NotificationType::dontSendNotification);
  if (pageStack->getCurrentPage()->getName() == "SSID List Page") {
    pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}
