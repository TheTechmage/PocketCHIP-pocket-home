#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class GridPage : public Component {
public:
  GridPage();
  ~GridPage() override;
  
  Array<Component *> items;
  
  // WIP: move up
  static constexpr int gridCols = 3;
  static constexpr int gridRows = 2;
  
  void addItem(Component *item);
  void resized() override;
  // WIP: make these private once the rest gets pulled in
  // TODO: number of rows should be controlled by the gridRows var
  ScopedPointer<Component> gridRow1;
  ScopedPointer<Component> gridRow2;
  Component* itemsRow1[gridCols];
  Component* itemsRow2[gridCols];
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridPage)
};

class Grid : public Component {
public:
  Grid();
  ~Grid() override;
  
  Array<Component *> items;
  
  OwnedArray<GridPage> pages;
  GridPage* page = nullptr;
  
  void addItem(Component *item);
  void showNextPage();
  void resized() override;
private:
  StretchableLayoutManager rowLayout;
  StretchableLayoutManager colLayout;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Grid)
};

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

  float itemScaleMin = 0.83f;
  float itemScaleMax = 0.83f;

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
  int itemSpacing;
        
  ScopedPointer<Component> dragModal;
        
  ScopedPointer<Grid> grid = nullptr;

  AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries> position;
  Rectangle<int> itemBounds;

  void setItemBoundsToFit();
  void updateItemTransforms();
  void updateItemSpacing();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrainComponent)
};
