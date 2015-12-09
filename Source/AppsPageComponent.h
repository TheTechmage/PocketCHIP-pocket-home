#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TrainComponent.h"

class AppsPageComponent : public Component, public Button::Listener {
public:
  ScopedPointer<TrainComponent> train;
  OwnedArray<Component> trainIcons;

  AppsPageComponent();
  ~AppsPageComponent();

  void paint(Graphics &) override;
  void resized() override;

  void addAndOwnIcon(const String &name, Component *icon);
  ImageButton *createAndOwnIcon(const String &name, const String &iconPath);
  Array<ImageButton *> createIconsFromJsonArray(const var &json);

  void buttonClicked(Button *) override;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppsPageComponent)
};
