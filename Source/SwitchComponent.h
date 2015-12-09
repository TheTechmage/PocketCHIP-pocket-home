#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SwitchComponent : public ToggleButton {
public:
  SwitchComponent();
  ~SwitchComponent();

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
  void resized() override;

private:
  Rectangle<int> pillBounds;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwitchComponent)
};
