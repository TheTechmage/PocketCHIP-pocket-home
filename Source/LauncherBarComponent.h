#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LauncherBarButton : public ImageButton {
public:
  LauncherBarButton(const String &name, const Image &image);
  float imageHeight = 1.f;
};

class LauncherBarComponent : public Component, public ButtonListener {
public:
  OwnedArray<ImageButton> buttons;

  StretchableLayoutManager layout;
  bool layoutDirty = false;

  ScopedPointer<Drawable> tempIcon;

  LauncherBarComponent();
  ~LauncherBarComponent();

  void resized() override;
  void buttonClicked(Button *button) override;

  void addButton(const String &name, const String &iconPath);
  void addButtonsFromJsonArray(const Array<var> &buttons);

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LauncherBarComponent)
};
