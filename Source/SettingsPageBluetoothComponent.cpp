#include "SettingsPageBluetoothComponent.h"
#include "Main.h"
#include "Utils.h"

SettingsPageBluetoothComponent::SettingsPageBluetoothComponent() {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  auto deviceListJson = parseDeviceListJson("../../assets/bluetooth.json");
  auto deviceListArray = deviceListJson.getArray();
  for (const auto &btDevice : *deviceListArray) {
    BTDevice device;
    device.name = btDevice["name"].toString();
    device.mac = btDevice["mac"].toString();
    device.connected = btDevice["connected"];
    device.paired = btDevice["paired"];
    deviceList.push_back(device);
  }


  ScopedPointer<XmlElement> btSvg = XmlDocument::parse(BinaryData::bluetoothIcon_svg);
  btIcon = Drawable::createFromSVG(*btSvg);
  addAndMakeVisible(btIcon);

  ScopedPointer<XmlElement> checkSvg = XmlDocument::parse(BinaryData::check_svg);
  checkIcon = Drawable::createFromSVG(*checkSvg);

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

  deviceListBox = new ListBox();
  deviceListBox->setModel(this);
  deviceListBox->setMultipleSelectionEnabled(false);
  deviceListPage->addAndMakeVisible(deviceListBox);

  // create device connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->addListener(this);
  connectionPage->addAndMakeVisible(connectionButton);

//  pageStack->setVisible(false);
}


SettingsPageBluetoothComponent::~SettingsPageBluetoothComponent() {}

void SettingsPageBluetoothComponent::paint(Graphics &g) {}

void SettingsPageBluetoothComponent::setBluetoothEnabled(bool enabled) {
  if (enabled)
    pageStack->pushPage(deviceListPage, PageStackComponent::kTransitionNone);
  else
    pageStack->clear(PageStackComponent::kTransitionNone);
}

void SettingsPageBluetoothComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  backButton->setBounds(10, 10, 62, 62);

  pageStack->setBounds(pageBounds);
  deviceListBox->setBounds(0, 0, pageBounds.getWidth(), pageBounds.getHeight());
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 24);

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
  if (button == connectionButton) {
    auto &curDevice = deviceList[currentDeviceIndex];
    curDevice.connected = !curDevice.connected;
    pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
  }
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

var SettingsPageBluetoothComponent::parseDeviceListJson(const String &path) {
  auto btDeviceListFile = absoluteFileFromPath(path);
  auto btDeviceJson = JSON::parse(btDeviceListFile);
  if (!btDeviceJson) {
    std::cerr << "Could not read bluetooth.json file from " << btDeviceListFile.getFullPathName()
              << std::endl;
  }
  return btDeviceJson;
}

int SettingsPageBluetoothComponent::getNumRows() {
  return deviceList.size();
}

void SettingsPageBluetoothComponent::paintListBoxItem(int rowNumber, Graphics &g, int width,
                                                      int height, bool rowIsSelected) {
  const auto &device = deviceList[rowNumber];
  auto contentHeight = height * 0.7f;

  if (rowIsSelected) g.fillAll(Colours::lightgrey);

  if (device.connected) {
    checkIcon->setSize(height, height);
    checkIcon->drawWithin(g,
                          Rectangle<float>(width - (height * 6), 3, contentHeight, contentHeight),
                          RectanglePlacement::fillDestination, 1.0f);
  }

  g.setFont(contentHeight);
  g.drawText(device.name, 5, 0, width, height, Justification::centredLeft, true);
}

void SettingsPageBluetoothComponent::listBoxItemClicked(int rowNumber, const MouseEvent &) {
  currentDeviceIndex = rowNumber;

  const auto &device = deviceList[rowNumber];

  connectionLabel->setText(device.name + "\n" + device.mac,
                           juce::NotificationType::dontSendNotification);

  connectionButton->setButtonText(device.connected ? "Disconnect" : "Connect");

  if (pageStack->getCurrentPage()->getName() == "Device List Page") {
    pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}
