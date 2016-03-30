#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class TrainComponent
    : public Component,
      public AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries>::Listener {
public:
  // TODO: grid orientation is very poorly shoehorned into the Train.
  // come up with a cleaner implementation
  enum Orientation {
    kOrientationHorizontal,
    kOrientationVertical,
    kOrientationGrid
  };

  Orientation orientation = kOrientationHorizontal;

  int itemWidth = 0;
  int itemHeight = 0;

  float itemScaleMin = 1.0f;
  float itemScaleMax = 1.0f;

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

  AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries> position;
  Rectangle<int> itemBounds;

  int itemSpacing;
  int gridCols = 2;
  int gridRows = 2;

  void setItemBoundsToFit();
  void updateItemTransforms();
  void updateItemSpacing();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrainComponent)
};
