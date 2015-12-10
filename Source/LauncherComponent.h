#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "LauncherBarComponent.h"
#include "PageStackComponent.h"

class LauncherComponent : public Component, private Button::Listener {
public:
  ScopedPointer<LauncherBarComponent> categoryButtons;

  OwnedArray<Component> pages;
  ScopedPointer<PageStackComponent> pageStack;
  HashMap<String, Component *> pagesByName;

  StretchableLayoutManager categoryButtonLayout;

  LauncherComponent(const var &configJson);
  ~LauncherComponent();

  void paint(Graphics &) override;
  void resized() override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LauncherComponent)

  void buttonClicked(Button *) override;
};
