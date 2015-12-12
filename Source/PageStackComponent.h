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

  void pushPage(Component *page, Transition transition);
  void swapPage(Component *page, Transition transition);
  void popPage(Transition transition);

  int getDepth() const;

  Component *getCurrentPage();

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageStackComponent)

  Array<Component *> stack;

  void transitionIn(Component *component, Transition transition, int durationMillis,
                    bool reverse = false);
  void transitionOut(Component *component, Transition transition, int durationMillis,
                     bool reverse = false);
};
