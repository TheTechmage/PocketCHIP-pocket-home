#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SwitchComponent.h"
#include "PageStackComponent.h"

class SettingsPageWifiComponent : public Component, private Button::Listener, private ListBoxModel {
public:
  SettingsPageWifiComponent();
  ~SettingsPageWifiComponent();

  bool wifiEnabled = false, wifiConnected = false;

  ScopedPointer<PageStackComponent> pageStack;

  ScopedPointer<ImageButton> backButton;

  ScopedPointer<Component> ssidListPage;
  ScopedPointer<ListBox> ssidList;
  ScopedPointer<ListBoxModel> ssidListModel;

  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;
  ScopedPointer<TextEditor> passwordEditor;
  ScopedPointer<TextButton> connectionButton;

  ScopedPointer<Drawable> wifiIcon;
  ScopedPointer<SwitchComponent> switchComponent;

  void setWifiEnabled(bool enabled);

  void paint(Graphics &) override;
  void resized() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageWifiComponent)

  int getNumRows() override;
  void paintListBoxItem(int rowNumber, Graphics &g, int width, int height,
                        bool rowIsSelected) override;
  void listBoxItemClicked(int row, const MouseEvent &) override;

  void buttonClicked(Button *) override;
  void buttonStateChanged(Button *) override;
};
