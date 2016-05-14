#include "PageStackComponent.h"
#include "Utils.h"

PageStackComponent::PageStackComponent() {
  setInterceptsMouseClicks(false, true);
}

PageStackComponent::~PageStackComponent() {}

void PageStackComponent::paint(Graphics &g) {}

void PageStackComponent::resized() {
  if (!stack.empty()) {
    stack.getLast()->setBounds(getLocalBounds());
  }
}

int PageStackComponent::getDepth() const {
  return stack.size();
}

void PageStackComponent::pushPage(Component *page, Transition transition) {
  auto bounds = getLocalBounds();
  if (!stack.empty()) {
    transitionOut(stack.getLast(), transition, transitionDurationMillis);
  }
  stack.add(page);
  transitionIn(page, transition, transitionDurationMillis);
}

void PageStackComponent::swapPage(Component *page, Transition transition) {
  if (!stack.empty()) {
    transitionOut(stack.getLast(), transition, transitionDurationMillis);
    stack.removeLast();
  }
  stack.add(page);
  transitionIn(page, transition, transitionDurationMillis);
}

void PageStackComponent::popPage(Transition transition) {
  if (!stack.empty()) {
    transitionOut(stack.getLast(), transition, transitionDurationMillis, true);
    stack.removeLast();
    if (!stack.empty()) {
      transitionIn(stack.getLast(), transition, transitionDurationMillis, true);
    }
  }
}

void PageStackComponent::insertPage(Component *page, int idx) {
  stack.insert(idx, page);
}

void PageStackComponent::removePage(int idx) {
  stack.remove(idx);
}

void PageStackComponent::clear(Transition transition) {
  if (!stack.empty()) {
    transitionOut(stack.getLast(), transition, transitionDurationMillis, true);
  }
  stack.clear();
}

void PageStackComponent::transitionIn(Component *component, Transition transition,
                                      int durationMillis, bool reverse) {
  addAndMakeVisible(component);
  auto bounds = getLocalBounds();
  switch (transition) {
    case kTransitionTranslateHorizontal: {
      float dir = reverse ? -1.0f : 1.0f;
      component->setBounds(bounds.translated(bounds.getWidth() * dir, 0));
      animateTranslation(component, 0, 0, 1.0f, durationMillis);
      break;
    } case kTransitionTranslateHorizontalLeft: {
      float dir = reverse ? 1.0f : -1.0f;
      component->setBounds(bounds.translated(bounds.getWidth() * dir, 0));
      animateTranslation(component, 0, 0, 1.0f, durationMillis);
    } break;
    default: {
      component->setBounds(bounds);
      component->setVisible(true);
    }
  }

  component->setEnabled(true);
}

void PageStackComponent::transitionOut(Component *component, Transition transition,
                                       int durationMillis, bool reverse) {
  switch (transition) {
    case kTransitionTranslateHorizontal: {
      auto bounds = getLocalBounds();
      float dir = reverse ? 1.0f : -1.0f;
      animateTranslation(component, bounds.getWidth() * dir, 0, 1.0f, durationMillis);
      break;
    } case kTransitionTranslateHorizontalLeft: {
      auto bounds = getLocalBounds();
      float dir = reverse ? -1.0f : 1.0f;
      animateTranslation(component, bounds.getWidth() * dir, 0, 1.0f, durationMillis);
      break;
    } break;
    default: {}
  }

  component->setEnabled(false);
  removeChildComponent(component);
}

Component *PageStackComponent::getCurrentPage() {
  return stack.getLast();
}
