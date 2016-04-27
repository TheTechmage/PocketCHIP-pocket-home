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