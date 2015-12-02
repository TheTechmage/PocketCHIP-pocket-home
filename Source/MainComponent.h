#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageComponent.h"

class MainContentComponent : public Component {
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)

public:
  std::unique_ptr<SettingsPageComponent> settingsPage;

  MainContentComponent();
  ~MainContentComponent();

  void paint(Graphics &);
  void resized();
};
