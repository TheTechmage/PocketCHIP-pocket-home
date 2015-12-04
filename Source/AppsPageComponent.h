#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TrainComponent.h"

#include <memory>

class AppsPageComponent : public Component {
public:
  ScopedPointer<TrainComponent> train;
  OwnedArray<Component> trainIcons;

  AppsPageComponent();
  ~AppsPageComponent();

  void paint(Graphics &);
  void resized();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppsPageComponent)
};
