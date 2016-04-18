#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"

class LibraryPageComponent : public AppListComponent {
public:
  LibraryPageComponent();
  ~LibraryPageComponent();
  
  ScopedPointer<ImageButton> backButton;
  
  void paint(Graphics &g) override;
  void resized() override;
  
  void buttonClicked(Button *button) override;
private:
  Colour bgColor;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryPageComponent)
};