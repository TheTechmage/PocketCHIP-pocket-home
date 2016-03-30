#include "TrainComponent.h"
#include "Utils.h"

TrainComponent::TrainComponent() {
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

void TrainComponent::addItem(Component *item) {
  items.add(item);
  addAndMakeVisible(item);
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
  int row = 0;
  int col = 0;

  for (auto item : items) {
    auto s = mix(itemScaleMax, itemScaleMin, smoothstep(0.0f, 1.0f, std::abs(p)));
    auto c = item->getBounds().getCentre();

    auto xf = AffineTransform::identity.scaled(s, s, c.getX(), c.getY());

    switch (orientation) {
      case kOrientationGrid: {
        xf = xf.translated(std::floor(itemBounds.getWidth() * col), std::floor(itemBounds.getHeight() * row));

        // TODO: multiple pages
        col += 1;
        if (col == gridCols) {
          col = 0;
          row += 1;
        }
      } break;
      case kOrientationHorizontal: {
        xf = xf.translated(std::floor(itemSpacing * p), 0);
      } break;
      case kOrientationVertical: {
        xf = xf.translated(0, std::floor(itemSpacing * p));
      } break;
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
