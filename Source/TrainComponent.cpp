#include "TrainComponent.h"
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
      rowLayout.setItemLayout(layoutIdx, -rowSpacerProp, -rowSpacerProp, -rowSpacerProp);
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

TrainComponent::TrainComponent(Orientation orientation_) {
  orientation = orientation_;
  
  if (kOrientationGrid == orientation) {
    grid = new Grid(3,2);
    addAndMakeVisible(grid);
  }

  position.setPosition(0.0);
  position.addListener(this);

  dragModal = new Component();
  dragModal->setAlwaysOnTop(true);
  dragModal->setInterceptsMouseClicks(true, false);
  addChildComponent(dragModal);
  
  addMouseListener(this, true);
}

TrainComponent::~TrainComponent() {}

void TrainComponent::paint(Graphics &g) {}

void TrainComponent::resized() {
  const auto& bounds = getLocalBounds();
  
  if (kOrientationGrid == orientation) {
    grid->setBounds(bounds);
  }
  else {
    dragModal->setBounds(bounds);
    
    updateItemSpacing();
    setItemBoundsToFit();
    updateItemTransforms();
  }
}

void TrainComponent::childrenChanged() {}

void TrainComponent::mouseDown(const MouseEvent &e) {
  if (orientation == kOrientationGrid) return;
  
  updateItemSpacing();
  position.beginDrag();
}

void TrainComponent::mouseDrag(const MouseEvent &e) {
  if (orientation == kOrientationGrid) return;
  
  if (!dragModal->isVisible() && e.getDistanceFromDragStart() > 5) {
    dragModal->setVisible(true);
  }
  if (dragModal->isVisible()) {
    double offset = orientation == kOrientationHorizontal ? e.getOffsetFromDragStart().getX()
                                                          : e.getOffsetFromDragStart().getY();
    position.drag(offset / itemSpacing);
  }
}

void TrainComponent::mouseUp(const MouseEvent &e) {
  if (orientation == kOrientationGrid) return;
  
  position.endDrag();
  dragModal->setVisible(false);
}

void TrainComponent::positionChanged(
    AnimatedPosition<AnimatedPositionBehaviours::SnapToPageBoundaries> &position,
    double newPosition) {
  updateItemTransforms();
}

bool TrainComponent::hasPrevPage() {
  if (kOrientationGrid == orientation)
    return grid->hasPrevPage();
  else
    return false;
}

bool TrainComponent::hasNextPage() {
  if (kOrientationGrid == orientation)
    return grid->hasNextPage();
  else
    return false;
}

void TrainComponent::showPrevPage() {
  if (hasPrevPage())
    grid->showPrevPage();
}

void TrainComponent::showNextPage() {
  if (hasNextPage())
    grid->showNextPage();
}

void TrainComponent::addItem(Component *item) {
  if (kOrientationGrid == orientation) {
    grid->addItem(item);
  }
  else {
    items.add(item);
    addAndMakeVisible(item);
    position.setLimits(Range<double>(1 - items.size(), 0.0));
  }
}

void TrainComponent::setItemBoundsToFit() {
  auto b = getLocalBounds();
  for (auto item : items) {
    item->setBounds(itemBounds);
  }
}

void TrainComponent::updateItemTransforms() {
  float p = position.getPosition();

  for (auto item : items) {
    auto s = mix(itemScaleMax, itemScaleMin, smoothstep(0.0f, 1.0f, std::abs(p)));
    auto c = item->getBounds().getCentre();
    
    auto xf = AffineTransform::identity.scaled(s, s, c.getX(), c.getY());
    
    if (orientation == kOrientationHorizontal) {
      xf = xf.translated(std::floor(itemSpacing * p), 0);
    }
    else if (orientation == kOrientationVertical) {
      xf = xf.translated(0, std::floor(itemSpacing * p));
    }
    
    item->setTransform(xf);
    p += 1.0f;
  }
}

void TrainComponent::updateItemSpacing() {
  auto bounds = getLocalBounds();

  int defaultItemSize =
      orientation == kOrientationHorizontal ? bounds.getHeight() : bounds.getWidth();

  int w = itemWidth > 0.0 ? itemWidth : defaultItemSize;
  int h = itemHeight > 0.0 ? itemHeight : defaultItemSize;

  itemBounds = { 0, 0, w, h };
  itemSpacing =
      orientation == kOrientationHorizontal ? itemBounds.getWidth() : itemBounds.getHeight();
}
