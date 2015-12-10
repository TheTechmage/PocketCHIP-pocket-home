#include "SettingsPageBluetoothComponent.h"
#include "Utils.h"

SettingsPageBluetoothComponent::SettingsPageBluetoothComponent() {}

SettingsPageBluetoothComponent::~SettingsPageBluetoothComponent() {}

void SettingsPageBluetoothComponent::paint(Graphics &g) {
  g.fillAll(Colours::white); // clear the background

  g.setColour(Colours::grey);
  g.drawRect(getLocalBounds(), 1); // draw an outline around the component

  g.setColour(Colours::lightblue);
  g.setFont(14.0f);
  g.drawText("SettingsPageBluetoothComponent", getLocalBounds(), Justification::centred,
             true); // draw some placeholder text
}

void SettingsPageBluetoothComponent::resized() {}
