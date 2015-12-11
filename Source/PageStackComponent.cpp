#include "PageStackComponent.h"
#include "Utils.h"

PageStackComponent::PageStackComponent() {
  setInterceptsMouseClicks(false, true);
}
PageStackComponent::~PageStackComponent() {}

void PageStackComponent::paint(Graphics &g) {}
void PageStackComponent::resized() {}

void PageStackComponent::pushPage(Component *page, Transition transtion) {
  auto bounds = getLocalBounds();
  if (!stack.empty()) {
    transitionOut(stack.getLast(), transtion, transitionDurationMillis);
  }
  stack.add(page);
  transitionIn(page, transtion, transitionDurationMillis);
}

void PageStackComponent::swapPage(Component *page, Transition transtion) {
  popPage(transtion);
  stack.add(page);
  transitionIn(page, transtion, transitionDurationMillis);
}

void PageStackComponent::popPage(Transition transtion) {
  if (!stack.empty()) {
    transitionOut(stack.getLast(), transtion, transitionDurationMillis);
    stack.removeLast();
  }
}

void PageStackComponent::transitionIn(Component *component, Transition transtion,
                                      int durationMillis) {
  addAndMakeVisible(component);

  auto bounds = getLocalBounds();
  switch (transtion) {
    case kTransitionTranslateHorizontal: {
      component->setBounds(bounds.translated(bounds.getWidth(), 0));
      animateTranslation(component, 0, 0, 1.0f, durationMillis);
    } break;
    default: {
      component->setBounds(bounds);
      component->setVisible(true);
    }
  }
}

void PageStackComponent::transitionOut(Component *component, Transition transtion,
                                       int durationMillis) {
  switch (transtion) {
    case kTransitionTranslateHorizontal: {
      auto bounds = getLocalBounds();
      animateTranslation(component, -bounds.getWidth(), 0, 1.0f, durationMillis);
    } break;
    default: {}
  }

  removeChildComponent(component);
}

Component *PageStackComponent::getCurrentPage() {
  return stack.getLast();
}
