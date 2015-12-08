#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LauncherBarComponent : public Component, public ButtonListener {
public:
  int buttonPadding = 10;

  OwnedArray<DrawableButton> buttons;
  StretchableLayoutManager layout;

  ScopedPointer<Drawable> tempIcon, tempIconSelected;

  LauncherBarComponent(const Array<var> &categories, int buttonSize);
  ~LauncherBarComponent();

  void paint(Graphics &) override;
  void resized() override;
  void buttonClicked(Button *button) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LauncherBarComponent)
};
