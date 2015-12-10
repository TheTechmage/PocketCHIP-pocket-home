#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SwitchComponent.h"

class SettingsPageWifiComponent : public Component, private Button::Listener {
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageWifiComponent)

public:
  SettingsPageWifiComponent();
  ~SettingsPageWifiComponent();

  ScopedPointer<Component> ssidListPanel;
  ScopedPointer<ListBox> ssidList;
  ScopedPointer<ListBoxModel> ssidListModel;

  Boolean wifiEnabled = false;
  ScopedPointer<Drawable> wifiIcon;
  ScopedPointer<SwitchComponent> switchComponent;

  void setWifiEnabled(Boolean enabled);
  void paint(Graphics &) override;
  void resized() override;

private:
  void buttonClicked(Button *) override;
  void buttonStateChanged(Button *) override;
};
