#include "SettingsPageBluetoothComponent.h"
#include "Main.h"
#include "Utils.h"

BluetoothDeviceListItem::BluetoothDeviceListItem(const BTDevice &device, BTIcons *icons)
: Button{ device.name }, device(device), icons{ icons } {}

void BluetoothDeviceListItem::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bounds = getLocalBounds();
  auto w = bounds.getWidth(), h = bounds.getHeight();

  auto iconBounds = Rectangle<float>(w - h, 0, h, h);

  auto contentHeight = h * 0.7f;

  if (device.connected) {
    icons->checkIcon->setSize(h, h);
    icons->checkIcon->drawWithin(g, Rectangle<float>(w - h, 3, contentHeight, contentHeight),
                                 RectanglePlacement::fillDestination, 1.0f);
  }

  g.setFont(Font(getLookAndFeel().getTypefaceForFont(Font())));
  g.setFont(h);
  g.setColour(findColour(DrawableButton::textColourId));
  g.drawText(device.name, 5, 0, w, h, Justification::centredLeft, true);
}

SettingsPageBluetoothComponent::SettingsPageBluetoothComponent() {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  // create device list "page"
  deviceListPage = new TrainComponent();
  deviceListPage->setOrientation(TrainComponent::kOrientationVertical);
  deviceListPage->itemHeight = 32;
  deviceListPage->itemScaleMin = 0.9f;

  auto deviceListJson = parseDeviceListJson("../../assets/bluetooth.json");
  auto deviceListArray = deviceListJson.getArray();
  for (const auto &btDevice : *deviceListArray) {
    BTDevice device;
    device.name = btDevice["name"].toString();
    device.mac = btDevice["mac"].toString();
    device.connected = btDevice["connected"];
    device.paired = btDevice["paired"];
    deviceList.push_back(device);

    auto item = new BluetoothDeviceListItem(device, &icons);
    item->addListener(this);
    deviceListItems.add(item);
    deviceListPage->addItem(item);
  }

  btIcon = new ImageComponent("WiFi Icon");
  btIcon->setImage(
      ImageFileFormat::loadFrom(BinaryData::bluetoothIcon_png, BinaryData::bluetoothIcon_pngSize));
  addAndMakeVisible(btIcon);

  // create back button
  ScopedPointer<Drawable> backButtonDrawable =
      Drawable::createFromImageData(BinaryData::backIcon_png, BinaryData::backIcon_pngSize);
  backButton = createImageButtonFromDrawable("Back", *backButtonDrawable);
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  // create device connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionPage->addAndMakeVisible(connectionLabel);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->addListener(this);
  connectionPage->addAndMakeVisible(connectionButton);

  icons.checkIcon = Drawable::createFromImageData(BinaryData::check_png, BinaryData::check_pngSize);
}

SettingsPageBluetoothComponent::~SettingsPageBluetoothComponent() {}

void SettingsPageBluetoothComponent::paint(Graphics &g) {}


void SettingsPageBluetoothComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  backButton->setBounds(10, 10, 62, 62);

  pageStack->setBounds(pageBounds);
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 24);

  connectionLabel->setBounds(10, 90, pageBounds.getWidth() - 20, 24);

  btIcon->setBounds(bounds.getX() + 2, bounds.getHeight() / 2.0f - 40, 80, 80);

  if (!init) { // TODO: Stupid shim to layout page correctly.
               // Should be in Constructor, or not at all.
    init = true;
    pageStack->pushPage(deviceListPage, PageStackComponent::kTransitionNone);
  }
}

void SettingsPageBluetoothComponent::buttonClicked(Button *button) {
  if (button == connectionButton && selectedDevice) {
    selectedDevice->connected = !selectedDevice->connected;
    pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
  }

  if (button == backButton) {
    if (pageStack->getDepth() > 1) {
      pageStack->popPage(PageStackComponent::kTransitionTranslateHorizontal);
    } else {
      getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    }
  }


  auto btButton = dynamic_cast<BluetoothDeviceListItem *>(button);
  if (btButton) {
    selectedDevice = &btButton->device;
    connectionButton->setButtonText(selectedDevice->connected ? "Disconnect" : "Connect");
    connectionLabel->setText(selectedDevice->name + "\n" + selectedDevice->mac,
                             juce::NotificationType::dontSendNotification);
    pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}

var SettingsPageBluetoothComponent::parseDeviceListJson(const String &path) {
  auto btDeviceListFile = absoluteFileFromPath(path);
  auto btDeviceJson = JSON::parse(btDeviceListFile);
  if (!btDeviceJson) {
    std::cerr << "Could not read bluetootxh.json file from " << btDeviceListFile.getFullPathName()
              << std::endl;
  }
  return btDeviceJson;
}
