#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "PowerPageComponent.h"
#include "SwitchComponent.h"

class PowerPageComponent;

class PowerSpinnerTimer : public Timer {
public:
    PowerSpinnerTimer() {};
    void timerCallback();
    PowerPageComponent* powerComponent;
    int i = 0;
};

class PowerCategoryButton : public Button {
public:
  String displayText;

  PowerCategoryButton(const String &name);
  ~PowerCategoryButton() {}

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
  void resized() override;

  void setText(const String &text);

private:
  Rectangle<int> pillBounds;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerCategoryButton)
};

class PowerCategoryItemComponent : public Component, private Button::Listener {
public:
  ScopedPointer<DrawableButton> icon;
  ScopedPointer<PowerCategoryButton> button;
  //ScopedPointer<TextButton> powerOffButton;

  StretchableLayoutManager layout;

  PowerCategoryItemComponent(const String &name);
  ~PowerCategoryItemComponent() {}

  void paint(Graphics &g) override;
  void resized() override;

  void buttonClicked(Button *b) override;
  void buttonStateChanged(Button *b) override;
  void enablementChanged() override;

  

  virtual void enabledStateChanged(bool enabled) = 0;
  virtual void updateButtonText() = 0;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerCategoryItemComponent)
};

class PowerPageComponent : public Component, private Button::Listener {
public:

  StretchableLayoutManager verticalLayout;
    
    ScopedPointer<ImageButton> backButton;
    ScopedPointer<TextButton> powerOffButton;
    ScopedPointer<TextButton> rebootButton;
    ScopedPointer<TextButton> sleepButton;
    ScopedPointer<TextButton> felButton;
    ScopedPointer<Component> mainPage;
    ScopedPointer<ImageComponent> powerSpinner;
    PowerSpinnerTimer powerSpinnerTimer;
    Array<Image> launchSpinnerImages;

  PowerPageComponent();
  ~PowerPageComponent();
    
  bool buttonsDisabled;
  void paint(Graphics &g) override;
  void resized() override;
  void showPowerSpinner();
  void buttonClicked(Button *b) override;
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerPageComponent)
  Colour bgColor;
    String bgImage;
  ChildProcess child;
};

