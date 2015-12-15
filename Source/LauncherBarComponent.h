#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LauncherBarComponent : public Component, public ButtonListener {
public:
  int buttonSize;
  int buttonPadding = 10;

  OwnedArray<ImageButton> buttons;

  StretchableLayoutManager layout;
  bool layoutDirty = false;

  ScopedPointer<Drawable> tempIcon;

  LauncherBarComponent(int buttonSize);
  ~LauncherBarComponent();

  void paint(Graphics &) override;
  void resized() override;
  void buttonClicked(Button *button) override;

  void addCategory(const String &name, const String &iconPath);
  void addCategoriesFromJsonArray(const Array<var> &categories);

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LauncherBarComponent)
};
