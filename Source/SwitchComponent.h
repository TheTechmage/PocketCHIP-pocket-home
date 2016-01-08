#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SwitchComponent : public ToggleButton {
public:
  enum ColorIds { colorIdBackground = 0x100f000, colorIdHandle = 0x100f001, colorIdHandleOff = 0x100f002 };

  SwitchComponent();
  ~SwitchComponent();

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
  void resized() override;
  void clicked() override;

  ScopedPointer<DrawablePath> handle;
  ScopedPointer<Component> handleParent;
  Rectangle<int> handleBoundsOff, handleBoundsOn;

private:
  Rectangle<int> pillBounds;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwitchComponent)
};
