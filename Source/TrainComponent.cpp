#include "TrainComponent.h"
#include "Utils.h"

TrainComponent::TrainComponent() {
  position.setPosition(0.0);
  position.addListener(this);

  dragModal = new Component();
  dragModal->setAlwaysOnTop(true);
  dragModal->setInterceptsMouseClicks(true, false);
  addChildComponent(dragModal);
  
  gridRow1 = new Component();
  addAndMakeVisible(gridRow1);
  gridRow2 = new Component();
  addAndMakeVisible(gridRow2);

  addMouseListener(this, true);
}

TrainComponent::~TrainComponent() {}

void TrainComponent::paint(Graphics &g) {}

void TrainComponent::resized() {
  dragModal->setBounds(getLocalBounds());

  updateItemSpacing();
  setItemBoundsToFit();
  updateItemTransforms();
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

void TrainComponent::addGridItem(Component* item) {
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
  
  // set grid items layout constraints
  double rowProp = (1.0f/gridRows);
  double colProp = (1.0f/gridCols);
  for (int i = 0; i < gridRows; i++) {
    rowLayout.setItemLayout(i, -rowProp/4, -rowProp, -rowProp);
  }
  for (int i = 0; i < gridCols; i++) {
    colLayout.setItemLayout(i, -rowProp/4, -colProp, -colProp);
  }
  
  // add to view tree
  if (items.size() <= gridCols) {
    gridRow1->addAndMakeVisible(item);
  }
  else {
    gridRow2->addAndMakeVisible(item);
  }
}

void TrainComponent::addItem(Component *item) {
  if (kOrientationGrid) {
    addGridItem(item);
  }
  else {
    items.add(item);
    addAndMakeVisible(item);
  }
  position.setLimits(Range<double>(1 - items.size(), 0.0));
}

void TrainComponent::setOrientation(Orientation orientation_) {
  orientation = orientation_;
}

void TrainComponent::setItemBoundsToFit() {
  auto b = getLocalBounds();
  for (auto item : items) {
    item->setBounds(itemBounds);
  }
}

void TrainComponent::updateItemTransforms() {
  float p = position.getPosition();
  
  if (orientation == kOrientationGrid) {
    const auto& bounds = getLocalBounds();
    auto rowHeight = bounds.getHeight() / 2.0f;
    
    Component* rowComps[] = {gridRow1.get(), gridRow2.get()};
    // lay out components, size the rows first
    rowLayout.layOutComponents(rowComps, gridRows, bounds.getX(), bounds.getY(),
                               bounds.getWidth(), bounds.getHeight(),
                               true, true);
    // columns are laid out within rows, using the elements of the row
    colLayout.layOutComponents(itemsRow1, gridCols, bounds.getX(), bounds.getY(),
                               bounds.getWidth(), rowHeight,
                               false, true);
    colLayout.layOutComponents(itemsRow2, gridCols, bounds.getX(), bounds.getY(),
                               bounds.getWidth(), rowHeight,
                               false, true);
  }
  else {
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
