#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageComponent.h"

#include <memory>

class MainContentComponent : public Component, private Button::Listener {
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)

public:
  ScopedPointer<SettingsPageComponent> settingsPage;

  StretchableLayoutManager categoryButtonLayout;

  ScopedPointer<DrawableButton> appButton, gamesButton, settingsButton;

  ScopedPointer<LookAndFeel> lookAndFeel;

  ScopedPointer<TextButton> closeButton;

  MainContentComponent();
  ~MainContentComponent();

  void paint(Graphics &) override;
  void resized() override;

private:
  void buttonClicked(Button *) override;
};
