#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainContentComponent : public Component {
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)

public:
  MainContentComponent();
  ~MainContentComponent();

  void paint(Graphics &);
  void resized();
};
