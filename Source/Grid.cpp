#include "Grid.h"
#include "Utils.h"

GridPage::GridPage(int numCols, int numRows) :
  numCols(numCols),
  numRows(numRows),
  gridRows()
{
  for (int i = 0; i < numRows; i++) {
    gridRows.add(new Component());
    addAndMakeVisible(gridRows.getLast());
  }
}

bool GridPage::addItem(Component *item) {
  if (items.size() == numRows * numCols) return false;
  
  // add to flat list
  items.add(item);
  
  // add to view tree in row container
  int row = (items.size() - 1) / numCols;
  gridRows[row]->addAndMakeVisible(item);
  
  return true;
}

void GridPage::resized() {}

Grid::Grid(int numCols, int numRows) :
  numCols(numCols),
  numRows(numRows)
{
  page = new GridPage(numCols, numRows);
  pages.add(page);
  addAndMakeVisible(page);

  // mildly convoluted way of finding proportion of available height to give each row,
  // accounting for spacer rows which are relatively heighted based on this measure.
  // First measures row proportion without spacers.
  rowProp = (1.0f/numRows);
  // Then find an appropriate relative spacer proportion.
  rowSpacerProp = rowProp / 8.;
  // Reduce ideal row proportion by room taken up by spacers.
  double rowAmountWithoutSpacers = 1.0 - (rowSpacerProp * (numRows - 1));
  rowProp = (rowAmountWithoutSpacers / numRows);
  
  // columns lack spacers and are trivial to proportion.
  colProp = (1.0f/numCols);
  
  int layoutIdx = 0;
  for (int i = 0; i < numRows; i++) {
    rowLayout.setItemLayout(layoutIdx, -rowProp/4, -rowProp, -rowProp);
    layoutIdx++;
    // set size preference for spacer if there are rows left in the loop
    if ((i+1) < numRows) {
      rowLayout.setItemLayout(layoutIdx, 0, -rowSpacerProp, -rowSpacerProp);
      layoutIdx++;
    }
  }
  for (int i = 0; i < numCols; i++) {
    colLayout.setItemLayout(i, -colProp/4, -colProp, -colProp);
  }
}

void Grid::createPage() {
  pages.add(new GridPage(numCols, numRows));
}

void Grid::addItem(Component *item) {
  items.add(item);
  bool wasAdded = pages.getLast()->addItem(item);
  if (!wasAdded) {
    createPage();
    pages.getLast()->addItem(item);
  }
}

void Grid::resized() {
  const auto& bounds = getLocalBounds();
  
  // create row components list for use in stretch layout
  // include room for rows and spacers
  int numRowComps = (2*page->gridRows.size()) - 1;
  Component* rowComps[numRowComps];
  for (int i = 0, j = 0; i < numRows; i++) {
    rowComps[j++] = page->gridRows[i];
    // add spacer null component
    if ((i+1) < numRows) {
      rowComps[j++] = nullptr;
    }
  }

  // fill item row lists
  // create row components list for use in stretch layout
  Component* itemRows[numRows][numCols];
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      auto item = page->items[j + (i*numCols)];
      itemRows[i][j] = item;
    }
  }
  
  // get row height
  auto rowHeight = bounds.getHeight() * rowProp;
  
  // size from largest to smallest, stretchable
  // set page size to grid size
  page->setBounds(bounds);
  
  // lay out components, size the rows first
  rowLayout.layOutComponents(rowComps, numRowComps, bounds.getX(), bounds.getY(),
                             bounds.getWidth(), bounds.getHeight(),
                             true, true);
  
  // size items within rows, creating columns
  for (int i = 0; i < numRows; i++) {
    auto& itemsRow = itemRows[i];
    // columns are laid out within rows, using the elements of the row
    colLayout.layOutComponents(itemsRow, numCols, bounds.getX(), bounds.getY(),
                               bounds.getWidth(), rowHeight,
                               false, true);
  }
}

bool Grid::hasPrevPage() {
  return page != pages.getFirst();
}
bool Grid::hasNextPage() {
  return page != pages.getLast();
}

void Grid::showPageAtIndex(int idx) {
  removeChildComponent(page);
  page->setEnabled(false);
  page->setVisible(false);
  
  page = pages[idx];
  
  addAndMakeVisible(page);
  page->setVisible(true);
  page->setEnabled(true);
  resized();
}

void Grid::showPrevPage() {
  if (hasPrevPage()) {
    int i = pages.indexOf(page);
    showPageAtIndex(i-1);
  };
}

void Grid::showNextPage() {
  if (hasNextPage()) {
    int i = pages.indexOf(page);
    showPageAtIndex(i+1);
  };
}