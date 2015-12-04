#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TrainComponent.h"

class GamesPageComponent : public Component {
public:
  ScopedPointer<TrainComponent> train;
  OwnedArray<Component> trainIcons;

  GamesPageComponent();
  ~GamesPageComponent();

  void paint(Graphics &);
  void resized();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GamesPageComponent)
};
