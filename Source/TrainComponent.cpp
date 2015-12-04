#include "../JuceLibraryCode/JuceHeader.h"
#include "TrainComponent.h"

TrainComponent::TrainComponent() {
  setInterceptsMouseClicks(true, false);
}

TrainComponent::~TrainComponent() {}

void TrainComponent::paint(Graphics &g) {
//  g.setColour(Colours::yellow);
//  g.fillAll();
}

void TrainComponent::resized() {
  setChildrenBoundsToFit();
  updateChildrenTransforms();
}

void TrainComponent::childrenChanged() {
//  setChildrenBoundsToFit();
//  updateChildrenTransforms();
}

void TrainComponent::mouseDown(const MouseEvent &e) {
  positionDragStart = position;
}

void TrainComponent::mouseDrag(const MouseEvent &e) {
  position = positionDragStart + e.getOffsetFromDragStart().getX();
  updateChildrenTransforms();
}

void TrainComponent::setChildrenBoundsToFit() {
  auto b = getLocalBounds();

  for (int i = 0; i < getNumChildComponents(); ++i) {
    auto c = getChildComponent(i);
    c->setBounds(0, 0, 1, 1);
    c->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centred, false);
  }
}

void TrainComponent::updateChildrenTransforms() {
  float spacing = getLocalBounds().getHeight() * 1.25f;

  float positionMin = 0.0f;
  float positionMax = spacing * (getNumChildComponents() - 1);

  for (int i = 0; i < getNumChildComponents(); ++i) {
    auto xf = AffineTransform::translation(position + spacing * i, 0);
    getChildComponent(i)->setTransform(xf);
  }
}
