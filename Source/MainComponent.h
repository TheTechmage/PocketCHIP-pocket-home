#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "LauncherComponent.h"
#include "PageStackComponent.h"

class MainContentComponent : public Component {
public:
  ScopedPointer<LauncherComponent> launcher;
  ScopedPointer<LookAndFeel> lookAndFeel;
  ScopedPointer<ImageComponent> buttonPopup;

  ScopedPointer<PageStackComponent> pageStack;

  MainContentComponent(const var &configJson);
  ~MainContentComponent();

  void paint(Graphics &) override;
  void resized() override;
  
  void handleMainWindowInactive();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
