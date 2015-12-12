#include "SettingsPageBluetoothComponent.h"
#include "Main.h"
#include "Utils.h"

SettingsPageBluetoothComponent::SettingsPageBluetoothComponent() {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);
  
  ScopedPointer<XmlElement> btSvg = XmlDocument::parse(BinaryData::bluetoothIcon_svg);
  btIcon = Drawable::createFromSVG(*btSvg);
  addAndMakeVisible(btIcon);  
  
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
  
  // create device list "page"
  deviceListPage = new Component("Device List Page");

  deviceList = new ListBox();
  deviceList->setModel(this);
  deviceList->setMultipleSelectionEnabled(false);
  deviceListPage->addAndMakeVisible(deviceList);
  
  // create device connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  pageStack->setVisible(false);
}


SettingsPageBluetoothComponent::~SettingsPageBluetoothComponent() {}

void SettingsPageBluetoothComponent::paint(Graphics &g) { }

void SettingsPageBluetoothComponent::setBluetoothEnabled(bool enabled) {
  pageStack->setVisible(enabled);
  if (enabled) {
    pageStack->pushPage(deviceListPage, PageStackComponent::kTransitionNone);
  }
}

void SettingsPageBluetoothComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  backButton->setBounds(10, 10, 62, 62);

  pageStack->setBounds(pageBounds);
  deviceList->setBounds(0, 0, pageBounds.getWidth(), pageBounds.getHeight());

  connectionLabel->setBounds(10, 90, pageBounds.getWidth() - 20, 24);

  btIcon->setTopLeftPosition(bounds.getX(), bounds.getHeight() / 2.0f - 20);

  {
    auto t = switchComponent->getTransform();
    t = AffineTransform::identity.rotated(-float_Pi / 2.0)
            .translated(bounds.getX() + 75, bounds.getHeight() / 2.0f + 40);
    switchComponent->setTransform(t);
  }  
}

void SettingsPageBluetoothComponent::buttonClicked(Button *button) {
  // if (button == connectionButton) {
  //   if (wifiConnected) {
  //     passwordEditor->setVisible(true);
  //     connectionButton->setButtonText("Connect");
  //     wifiConnected = false;
  //     pageStack->pushPage(ssidListPage, PageStackComponent::kTransitionNone);
  //   } else {
  //     passwordEditor->setVisible(false);
  //     connectionButton->setButtonText("Disconnect");
  //     wifiConnected = true;
  //   }
  // }
  if (button == backButton) {
    if (pageStack->getDepth() > 1) {
      pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
    } else {
      getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    }
  }
}

void SettingsPageBluetoothComponent::buttonStateChanged(Button *button) {
  if (button == switchComponent && bluetoothEnabled != button->getToggleState()) {
    bluetoothEnabled = button->getToggleState();
    setBluetoothEnabled(bluetoothEnabled);
  }
}

int SettingsPageBluetoothComponent::getNumRows() {
  return 15;
}

void SettingsPageBluetoothComponent::paintListBoxItem(int rowNumber, Graphics &g, int width, int height,
                                                 bool rowIsSelected) {
  if (rowIsSelected) g.fillAll(Colours::lightgrey);
  g.setFont(height * 0.7f);
  g.drawText("BT Device " + String(rowNumber), 5, 0, width, height, Justification::centredLeft,
             true);
}

void SettingsPageBluetoothComponent::listBoxItemClicked(int rowNumber, const MouseEvent &) {
  connectionLabel->setText("BT Device " + String(rowNumber),
                           juce::NotificationType::dontSendNotification);
  if (pageStack->getCurrentPage()->getName() == "Device List Page") {
    pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}

