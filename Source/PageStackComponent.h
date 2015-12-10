#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class PageStackComponent : public Component {
public:
  int transitionDurationMillis = 200;

  PageStackComponent();
  ~PageStackComponent();

  void paint(Graphics &) override;
  void resized() override;

  enum Transition { kTransitionNone, kTransitionTranslateHorizontal };

  void pushPage(Component *page, Transition transtion);
  void swapPage(Component *page, Transition transtion);
  void popPage(Transition transtion);

  Component *getCurrentPage();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageStackComponent)

  Array<Component *> stack;

  void transitionIn(Component *component, Transition transtion, int durationMillis);
  void transitionOut(Component *component, Transition transtion, int durationMillis);
};
