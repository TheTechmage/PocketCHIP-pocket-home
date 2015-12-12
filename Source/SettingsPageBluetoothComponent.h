#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SwitchComponent.h"
#include "PageStackComponent.h"

struct BTDevice {
  String name = "";
  String mac = "";
  bool connected = false;
  bool paired = false;
};

class SettingsPageBluetoothComponent : public Component,
                                       private Button::Listener,
                                       private ListBoxModel {
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageBluetoothComponent)

public:
  SettingsPageBluetoothComponent();
  ~SettingsPageBluetoothComponent();

  bool bluetoothEnabled = false;
  int currentDeviceIndex;
  
  ScopedPointer<PageStackComponent> pageStack;
  
  ScopedPointer<ImageButton> backButton;
  ScopedPointer<Drawable> checkIcon;
  ScopedPointer<Drawable> btIcon;

  ScopedPointer<Component> deviceListPage;  
  ScopedPointer<ListBox> deviceListBox;
  ScopedPointer<ListBoxModel> deviceListModel;
  
  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;
  ScopedPointer<TextButton> connectionButton;

  ScopedPointer<SwitchComponent> switchComponent;
  
  var parseDeviceListJson(const String &path);
  std::vector<BTDevice> deviceList;  

  void setBluetoothEnabled(bool enabled);
  void paint(Graphics &) override;
  void resized() override;

private:
  int getNumRows() override;
  void paintListBoxItem(int rowNumber, Graphics &g, int width, int height,
                        bool rowIsSelected) override;
  void listBoxItemClicked(int row, const MouseEvent &) override;
  void buttonClicked(Button *) override;
  void buttonStateChanged(Button *) override;    
};
