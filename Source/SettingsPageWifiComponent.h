#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class SettingsPageWifiComponent    : public Component
{
public:
    SettingsPageWifiComponent();
    ~SettingsPageWifiComponent();

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsPageWifiComponent)
};
