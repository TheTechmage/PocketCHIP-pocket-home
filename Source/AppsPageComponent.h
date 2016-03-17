#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TrainComponent.h"

class AppIconButton : public DrawableButton {
public:
  AppIconButton(const String &label, const Drawable *image);

  Rectangle<float> getImageBounds() const override;
};

class AppsPageComponent : public Component, public Button::Listener {
public:
  ScopedPointer<TrainComponent> train;
  OwnedArray<Component> trainIcons;
  OwnedArray<DrawableImage> iconDrawableImages;

  AppsPageComponent();
  ~AppsPageComponent();

  void paint(Graphics &) override;
  void resized() override;

  void addAndOwnIcon(const String &name, Component *icon);
  DrawableButton *createAndOwnIcon(const String &name, const String &iconPath);
  Array<DrawableButton *> createIconsFromJsonArray(const var &json);

  void buttonClicked(Button *) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppsPageComponent)
};
