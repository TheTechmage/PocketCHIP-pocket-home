#include "SettingsPageBluetoothComponent.h"
#include "Main.h"
#include "Utils.h"

BluetoothDeviceListItem::BluetoothDeviceListItem(BluetoothDevice *device, BTIcons *icons)
: Button{ device->name }, device(device), icons{ icons } {}

void BluetoothDeviceListItem::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bounds = getLocalBounds();
  auto inset = bounds.reduced(6, 4);
  auto w = bounds.getWidth(), h = bounds.getHeight();
  auto iconBounds = Rectangle<float>(w - h, h/5.0, h*0.6, h*0.6);

  auto listOutline = Path();
  listOutline.addRoundedRectangle(inset.toFloat(), 10.0f);
  g.setColour(findColour(ListBox::ColourIds::backgroundColourId));
  g.fillPath(listOutline);

  if (device->connected) {
    icons->checkIcon->setSize(h, h);
    icons->checkIcon->drawWithin(g, iconBounds, RectanglePlacement::fillDestination, 1.0f);
  }

//  icons->arrowIcon->setSize(h, h);
//  icons->arrowIcon->drawWithin(g, Rectangle<float>(w - (h/8), contentHeight + 8, contentHeight, contentHeight),
//                               RectanglePlacement::fillDestination, 1.0f);


  g.setFont(Font(getLookAndFeel().getTypefaceForFont(Font())));
  g.setFont(h * 0.5);
  g.setColour(findColour(ListBox::ColourIds::textColourId));
  g.drawText(getName(), inset.reduced(h * 0.2, 0), Justification::centredLeft, true);
}

SettingsPageBluetoothComponent::SettingsPageBluetoothComponent() {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  // create device list "page"
  deviceListPage = new TrainComponent(TrainComponent::kOrientationVertical, 1, 4);
  deviceListPage->itemHeight = 50;
  deviceListPage->itemScaleMin = deviceListPage->itemScaleMax = 1.0;

  for (auto btDevice : getBluetoothStatus().devices) {
    auto item = new BluetoothDeviceListItem(btDevice, &icons);
    item->addListener(this);
    item->setTriggeredOnMouseDown(true);
    deviceListItems.add(item);
    deviceListPage->addItem(item);
  }

  //btIcon = new ImageComponent("BT Icon");
  //btIcon->setImage(
  //    ImageFileFormat::loadFrom(BinaryData::bluetoothIcon_png, BinaryData::bluetoothIcon_pngSize));
  //addAndMakeVisible(btIcon);

  // create back button
  backButton = createImageButton(
                                 "Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
  backButton->setTriggeredOnMouseDown(true);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);

  // create device connection "page"
  connectionPage = new Component("Connection Page");

  connectionLabel = new Label("Connected", "Connection Label");
  connectionLabel->setJustificationType(juce::Justification::centred);
  connectionLabel->setFont(26);
  connectionPage->addAndMakeVisible(connectionLabel);

  connectionButton = new TextButton("Connection Button");
  connectionButton->setButtonText("Connect");
  connectionButton->setTriggeredOnMouseDown(true);
  connectionButton->addListener(this);
  connectionPage->addAndMakeVisible(connectionButton);

  //icons.checkIcon = Drawable::createFromImageData(BinaryData::check_png, BinaryData::check_pngSize);
  icons.arrowIcon = Drawable::createFromImageFile(assetFile("backIcon.png"));
  auto xf = AffineTransform::identity.rotated(M_PI);
  icons.arrowIcon->setTransform(xf);
}

SettingsPageBluetoothComponent::~SettingsPageBluetoothComponent() {}

void SettingsPageBluetoothComponent::paint(Graphics &g) {}


void SettingsPageBluetoothComponent::resized() {
  auto bounds = getLocalBounds();
  auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());

  pageStack->setBounds(pageBounds);
  connectionLabel->setBounds(10, 70, pageBounds.getWidth() - 20, 60);
  connectionButton->setBounds(90, 160, pageBounds.getWidth() - 180, 50);
  btIcon->setBounds(-10, 0, 80, 80);
  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());

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
    selectedDevice = btButton->device;
    connectionButton->setButtonText(selectedDevice->connected ? "Disconnect" : "Connect");
    connectionLabel->setText(selectedDevice->name + "\n" + selectedDevice->macAddress,
                             juce::NotificationType::dontSendNotification);
    pageStack->pushPage(connectionPage, PageStackComponent::kTransitionTranslateHorizontal);
  }
}
