#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SwitchComponent.h"
#include "PageStackComponent.h"

class SettingsPageBluetoothComponent : public Component,
                                       private Button::Listener,
                                       private ListBoxModel {
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageBluetoothComponent)

public:
  SettingsPageBluetoothComponent();
  ~SettingsPageBluetoothComponent();
  
  bool bluetoothEnabled = false;
  
  ScopedPointer<PageStackComponent> pageStack;
  
  ScopedPointer<ImageButton> backButton;
  
  ScopedPointer<Component> deviceListPage;  
  ScopedPointer<ListBox> deviceList;
  ScopedPointer<ListBoxModel> deviceListModel;
  
  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;

  ScopedPointer<Drawable> btIcon;
  ScopedPointer<SwitchComponent> switchComponent;
  
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
