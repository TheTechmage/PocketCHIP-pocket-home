#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "LauncherComponent.h"
#include "PageStackComponent.h"

class MainContentComponent : public Component, private Button::Listener {
public:
  ScopedPointer<LauncherComponent> launcher;
  ScopedPointer<LookAndFeel> lookAndFeel;
  ScopedPointer<TextButton> closeButton;

  ScopedPointer<PageStackComponent> pageStack;

  MainContentComponent(const var &configJson);
  ~MainContentComponent();

  void paint(Graphics &) override;
  void resized() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)

  void buttonClicked(Button *) override;
};
