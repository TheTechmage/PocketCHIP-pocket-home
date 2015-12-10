#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SettingsPageBluetoothComponent    : public Component
{
public:
    SettingsPageBluetoothComponent();
    ~SettingsPageBluetoothComponent();
    
    ScopedPointer<Image> bluetoothIcon;

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsPageBluetoothComponent)
};
