#include "TrainComponent.h"
#include "Utils.h"

GridPage::GridPage() :
  gridRow1(new Component()),
  gridRow2(new Component())
{
  addAndMakeVisible(gridRow1);
  addAndMakeVisible(gridRow2);
}
GridPage::~GridPage() {}

bool GridPage::addItem(Component *item) {
  if (items.size() == gridRows * gridCols) return false;
  
  items.add(item);
  
  // build row lists for use in stretch layout
  for (int i = 0; i < gridRows; i++) {
    for (int j = 0; j < gridCols; j++) {
      auto item = items[j + (i*gridCols)];
      if (i == 0)
        itemsRow1[j] = item;
      else
        itemsRow2[j] = item;
    }
  }
  
  // add to view tree
  if (items.size() <= gridCols) {
    gridRow1->addAndMakeVisible(item);
  }
  else {
    gridRow2->addAndMakeVisible(item);
  }
  
  return true;
}

void GridPage::resized() {}

Grid::Grid() {
  page = new GridPage();
  pages.add(page);
  addAndMakeVisible(page);
  
  // FIXME: these should be static on this class, not child
  const auto gridRows = page->gridRows;
  const auto gridCols = page->gridCols;
  
  // mildly convoluted way of finding proportion of available height to give each row,
  // accounting for spacer rows which are relatively heighted based on this measure.
  // First measures row proportion without spacers.
  rowProp = (1.0f/gridRows);
  // Then find an appropriate relative spacer proportion.
  rowSpacerProp = rowProp / 8.;
  // Reduce ideal row proportion by room taken up by spacers.
  double rowAmountWithoutSpacers = 1.0 - (rowSpacerProp * (gridRows - 1));
  rowProp = (rowAmountWithoutSpacers / gridRows);
  
  // columns lack spacers and are trivial to proportion.
  colProp = (1.0f/gridCols);
  
  int layoutIdx = 0;
  for (int i = 0; i < gridRows; i++) {
    rowLayout.setItemLayout(layoutIdx, -rowProp/4, -rowProp, -rowProp);
    layoutIdx++;
    // set size preference for spacer if there are rows left in the loop
    if ((i+1) < gridRows) {
      rowLayout.setItemLayout(layoutIdx, -rowSpacerProp, -rowSpacerProp, -rowSpacerProp);
      layoutIdx++;
    }
  }
  for (int i = 0; i < gridCols; i++) {
    colLayout.setItemLayout(i, -colProp/4, -colProp, -colProp);
  }
}
Grid::~Grid() {}

void Grid::createPage() {
  pages.add(new GridPage());
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
  
  // build row list with spacers added
  Component* rowComps[] = {page->gridRow1.get(), nullptr, page->gridRow2.get()};
  int numRows = sizeof(rowComps) / sizeof(Component*);
  
  // build list of item rows
  Component** itemRows[] = {page->itemsRow1, page->itemsRow2};
  int numItemRows = sizeof(itemRows) / sizeof(Component**);
  
  // get row height and number of columns
  auto rowHeight = bounds.getHeight() * rowProp;
  const auto numCols = page->gridCols;
  
  // size from largest to smallest, stretchable
  // set page size to grid size
  page->setBounds(bounds);
  
  // lay out components, size the rows first
  rowLayout.layOutComponents(rowComps, numRows, bounds.getX(), bounds.getY(),
                             bounds.getWidth(), bounds.getHeight(),
                             true, true);
  
  // size items within rows, creating columns
  for (int i = 0; i < numItemRows; i++) {
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
    grid = new Grid();
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
