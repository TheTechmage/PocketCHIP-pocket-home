#include "../JuceLibraryCode/JuceHeader.h"
#include "TrainComponent.h"

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
  updateItemSpacing();
  position.beginDrag();
}

void TrainComponent::mouseDrag(const MouseEvent &e) {
  if (!dragModal->isVisible() && e.getDistanceFromDragStart() > 5) {
    dragModal->setVisible(true);
  }
  if (dragModal->isVisible()) {
    position.drag(e.getOffsetFromDragStart().getX() / itemSpacing);
  }
}

void TrainComponent::mouseUp(const MouseEvent &e) {
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

void TrainComponent::setItemBoundsToFit() {
  auto b = getLocalBounds();

  for (auto item : items) {
    item->setBounds(0, 0, 1, 1);
    item->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centred,
                         false);
  }
}

float smoothstep(float edge0, float edge1, float x) {
  x = std::min(std::max((x - edge0) / (edge1 - edge0), 0.0f), 1.0f);
  return x * x * (3.0f - 2.0f * x);
}

float mix(float a, float b, float t) {
  return t * (b - a) + a;
}

void TrainComponent::updateItemTransforms() {
  float p = position.getPosition();
  for (auto item : items) {
    auto s = mix(1.0f, 0.8f, smoothstep(0.0f, 1.0f, std::abs(p)));
    auto c = item->getBounds().getCentre();

    auto xf = AffineTransform::identity.scaled(s, s, c.getX(), c.getY())
                  .translated(std::floor(itemSpacing * p), 0);

    item->setTransform(xf);
    p += 1.0f;
  }
}

void TrainComponent::updateItemSpacing() {
  itemSpacing = getLocalBounds().getHeight();
}
