#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LauncherBarComponent : public Component
{
public:
  
  StretchableLayoutManager launcherButtonLayout;
  
  LauncherBarComponent();
  ~LauncherBarComponent();

  void paint (Graphics&);
  void resized();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LauncherBarComponent)
};
