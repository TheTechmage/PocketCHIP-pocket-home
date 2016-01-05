#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SwitchComponent.h"
#include "PageStackComponent.h"
#include "TrainComponent.h"

struct BTDevice {
  String name = "";
  String mac = "";
  bool connected = false;
  bool paired = false;
};

struct BTIcons {
  ScopedPointer<Drawable> checkIcon;
};

class BluetoothDeviceListItem : public Button {
public:
  BTDevice device;
  BTIcons *icons;

  BluetoothDeviceListItem(const BTDevice &device, BTIcons *icons);

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BluetoothDeviceListItem)
};

class SettingsPageBluetoothComponent : public Component, private Button::Listener {
public:
  SettingsPageBluetoothComponent();
  ~SettingsPageBluetoothComponent();

  bool init = false;

  bool bluetoothEnabled = true;
  int currentDeviceIndex;

  BTDevice* selectedDevice;

  ScopedPointer<PageStackComponent> pageStack;

  ScopedPointer<ImageButton> backButton;
  ScopedPointer<Drawable> checkIcon;
  ScopedPointer<ImageComponent> btIcon;

  ScopedPointer<TrainComponent> deviceListPage;
  OwnedArray<Component> deviceListItems;

  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;
  ScopedPointer<TextButton> connectionButton;

//  ScopedPointer<SwitchComponent> switchComponent;

  BTIcons icons;

  var parseDeviceListJson(const String &path);
  std::vector<BTDevice> deviceList;

  void paint(Graphics &) override;
  void resized() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageBluetoothComponent)

  void buttonClicked(Button *) override;
};
