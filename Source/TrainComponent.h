#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class GridPage : public Component {
public:
  GridPage(int numCols, int numRows);
  
  int numCols;
  int numRows;
  
  bool addItem(Component *item);
  void resized() override;

  Array<Component *> items;
  OwnedArray<Component> gridRows;
  
private:

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridPage)
};

class Grid : public Component {
public:
  Grid(int numCols, int numRows);
  
  int numCols;
  int numRows;
  Array<Component *> items;
  OwnedArray<GridPage> pages;
  GridPage* page = nullptr;
  
  void createPage();
  void addItem(Component *item);
  bool hasPrevPage();
  bool hasNextPage();
  void showPageAtIndex(int idx);
  void showPrevPage();
  void showNextPage();
  void resized() override;
private:
  StretchableLayoutManager rowLayout;
  StretchableLayoutManager colLayout;
  
  double rowProp;
  double rowSpacerProp;
  double colProp;
  
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

  TrainComponent(Orientation orientation_, int cols, int rows);
  ~TrainComponent();

  void paint(Graphics &) override;
  void resized() override;
  void childrenChanged() override;

  void mouseDown(const MouseEvent &e) override;
  void mouseDrag(const MouseEvent &e) override;
  void mouseUp(const MouseEvent &e) override;

  void positionChanged(AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries> &position,
                       double newPosition) override;
  
  bool hasPrevPage();
  bool hasNextPage();
  void showPrevPage();
  void showNextPage();

  void addItem(Component *item);

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
