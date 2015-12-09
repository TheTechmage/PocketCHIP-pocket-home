#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageWifiComponent.h"

SettingsPageWifiComponent::SettingsPageWifiComponent() {}

SettingsPageWifiComponent::~SettingsPageWifiComponent() {}

void SettingsPageWifiComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::lightblue);
    g.setFont (14.0f);
    g.drawText ("SettingsPageWifiComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void SettingsPageWifiComponent::resized() {}
