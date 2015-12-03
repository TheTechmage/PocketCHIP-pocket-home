#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageComponent.h"

#include <memory>

class MainContentComponent : public Component {
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)

public:
  ScopedPointer<SettingsPageComponent> settingsPage;

  StretchableLayoutManager categoryButtonLayout;

  ScopedPointer<DrawableButton> appButton, gamesButton, settingsButton;

  MainContentComponent();
  ~MainContentComponent();

  void paint(Graphics &);
  void resized();
};
