#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LauncherBarButton : public ImageButton {
public:
  LauncherBarButton(const String &name, const Image &image);

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
};

class LauncherBarComponent : public Component, public ButtonListener {
public:
  int buttonPadding = 10;

  OwnedArray<ImageButton> buttons;

  StretchableLayoutManager layout;
  bool layoutDirty = false;

  ScopedPointer<Drawable> tempIcon;

  LauncherBarComponent();
  ~LauncherBarComponent();

  void paint(Graphics &) override;
  void resized() override;
  void buttonClicked(Button *button) override;

  void addCategory(const String &name, const String &iconPath);
  void addCategoriesFromJsonArray(const Array<var> &categories);

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LauncherBarComponent)
};
