#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class TrainComponent : public Component {
public:
  float position = 0.0f;

  TrainComponent();
  ~TrainComponent();

  void paint(Graphics &) override;
  void resized() override;
  void childrenChanged() override;

  void mouseDown(const MouseEvent &e) override;
  void mouseDrag(const MouseEvent &e) override;

  void setChildrenBoundsToFit();
  void updateChildrenTransforms();

private:
  float positionDragStart;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrainComponent)
};
