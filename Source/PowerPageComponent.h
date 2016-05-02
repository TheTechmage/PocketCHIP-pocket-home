#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PowerPageComponent.h"
#include "SwitchComponent.h"
#include "PageStackComponent.h"

class PowerPageComponent;

class PowerSpinnerTimer : public Timer {
public:
    PowerSpinnerTimer() {};
    void timerCallback() override;
    PowerPageComponent* powerComponent;
    int i = 0;
};

class PowerPageComponent : public Component, private Button::Listener {
public:

  StretchableLayoutManager verticalLayout;
    
    ScopedPointer<ImageButton> backButton;
    ScopedPointer<TextButton> powerOffButton;
    ScopedPointer<TextButton> rebootButton;
    ScopedPointer<TextButton> sleepButton;
    ScopedPointer<TextButton> felButton;
    ScopedPointer<Label> buildNameLabel;
    ScopedPointer<Component> mainPage;
    ScopedPointer<ImageComponent> powerSpinner;
    PowerSpinnerTimer powerSpinnerTimer;
    Array<Image> launchSpinnerImages;
    HashMap<String, Component *> pagesByName;
  
  String buildName;
    ScopedPointer<PageStackComponent> pageStack;
    ScopedPointer<Component> felPage;
    

  PowerPageComponent();
  ~PowerPageComponent();
    
  void paint(Graphics &g) override;
  void resized() override;
  void showPowerSpinner();
  void buttonClicked(Button *b) override;
  void setSleep();
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerPageComponent)
  Colour bgColor;
  Image bgImage;
  String bgImagePath;
  ChildProcess child;
};

