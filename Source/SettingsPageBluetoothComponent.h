#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SwitchComponent.h"
#include "PageStackComponent.h"
#include "TrainComponent.h"
#include "Main.h"

struct BTIcons {
  ScopedPointer<Drawable> checkIcon;
};

class BluetoothDeviceListItem : public Button {
public:
  BluetoothDevice *device;
  BTIcons *icons;

  BluetoothDeviceListItem(BluetoothDevice *device, BTIcons *icons);

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

  BluetoothDevice *selectedDevice;

  ScopedPointer<PageStackComponent> pageStack;

  ScopedPointer<ImageButton> backButton;
  ScopedPointer<Drawable> checkIcon;
  ScopedPointer<ImageComponent> btIcon;

  ScopedPointer<TrainComponent> deviceListPage;
  OwnedArray<Component> deviceListItems;

  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;
  ScopedPointer<TextButton> connectionButton;

  BTIcons icons;

  void paint(Graphics &) override;
  void resized() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageBluetoothComponent)

  void buttonClicked(Button *) override;
};
