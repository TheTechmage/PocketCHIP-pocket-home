#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SwitchComponent.h"

class PowerFelPageComponent;

class PowerFelCategoryButton : public Button {
public:
    String displayText;
    
    PowerFelCategoryButton(const String &name);
    ~PowerFelCategoryButton() {}
    
    void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
    void resized() override;
    
    void setText(const String &text);
    
private:
    Rectangle<int> pillBounds;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerFelCategoryButton)
};

class PowerFelCategoryItemComponent : public Component, private Button::Listener {
public:
    ScopedPointer<DrawableButton> icon;
    ScopedPointer<PowerFelCategoryButton> button;
    
    StretchableLayoutManager layout;
    
    PowerFelCategoryItemComponent(const String &name);
    ~PowerFelCategoryItemComponent() {}
    
    void paint(Graphics &g) override;
    void resized() override;
    
    void buttonClicked(Button *b) override;
    void buttonStateChanged(Button *b) override;
    void enablementChanged() override;
  
    virtual void enabledStateChanged(bool enabled) = 0;
    virtual void updateButtonText() = 0;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerFelCategoryItemComponent)
};

class PowerFelPageComponent : public Component, private Button::Listener {
public:
  
    StretchableLayoutManager verticalLayout;
  
    ScopedPointer<Component> mainPage;
    ScopedPointer<Label> flashDescription;;
    ScopedPointer<Label> flashPrompt;
    ScopedPointer<TextButton> yesButton;
    ScopedPointer<TextButton> noButton;
    ScopedPointer<Label> flashInstructions;

    PowerFelPageComponent();
    ~PowerFelPageComponent();

    bool debounce;
    void paint(Graphics &g) override;
    void resized() override;
    void buttonStateChanged(Button*) override;
    void buttonClicked(Button*) override;
    void setSleep();
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PowerFelPageComponent)
    Colour bgColor;
    String bgImage;
    ChildProcess child;
};
