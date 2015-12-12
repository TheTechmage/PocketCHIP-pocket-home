#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class TrainComponent
    : public Component,
      public AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries>::Listener {
public:
  enum Orientation {
    kOrientationHorizontal,
    kOrientationVertical
  };

  Orientation orientation = kOrientationHorizontal;

  AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries> position;
  double itemSpacing;

  Array<Component *> items;

  TrainComponent();
  ~TrainComponent();

  void paint(Graphics &) override;
  void resized() override;
  void childrenChanged() override;

  void mouseDown(const MouseEvent &e) override;
  void mouseDrag(const MouseEvent &e) override;
  void mouseUp(const MouseEvent &e) override;

  void positionChanged(AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries> &position,
                       double newPosition) override;

  void addItem(Component *item);

  void setOrientation(Orientation orientation_);

private:
  ScopedPointer<Component> dragModal;

  void setItemBoundsToFit();
  void updateItemTransforms();
  void updateItemSpacing();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrainComponent)
};
