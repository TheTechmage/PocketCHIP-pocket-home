#include "PowerPageFelComponent.h"
#include "Main.h"
#include "Utils.h"
#include "PokeLookAndFeel.h"

#include <numeric>

PowerFelCategoryButton::PowerFelCategoryButton(const String &name)
: Button(name),
displayText(name)
{}

void PowerFelCategoryButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
    const auto& bounds = pillBounds;
    float borderThick = 4.0f;
    float radius = float(bounds.getHeight()) / 2.0f;
    
    g.setColour(Colours::white);
    if (isEnabled()) {
        g.drawRoundedRectangle(bounds.getX() + borderThick, bounds.getY() + borderThick,
                               bounds.getWidth() - 2*borderThick, bounds.getHeight()  - 2*borderThick,
                               radius, borderThick);
    }
    
    // TODO: write button text as grey if choice is completely unset?
    g.setFont(20);
    g.drawText(displayText, bounds.getX(), bounds.getY(),
               bounds.getWidth(), bounds.getHeight(),
               Justification::centred);
}

void PowerFelCategoryButton::resized() {
    pillBounds.setSize(getLocalBounds().getWidth(), 42);
    fitRectInRect(pillBounds, getLocalBounds(), Justification::centred, false);
}

void PowerFelCategoryButton::setText(const String &text) {
    displayText = text;
    repaint();
}

PowerFelCategoryItemComponent::PowerFelCategoryItemComponent(const String &name)
: icon{ new DrawableButton("icon", DrawableButton::ImageFitted) },
button{ new PowerFelCategoryButton(name) } {
    addAndMakeVisible(icon);
    addAndMakeVisible(button);
    button->setEnabled(false); // default to disabled state
}

void PowerFelCategoryItemComponent::paint(Graphics &g) {}

void PowerFelCategoryItemComponent::resized() {
    auto b = getLocalBounds();
    auto h = b.getHeight();
    
    int spacing = 10;
    int togWidth = h * 1.1f;
    
    layout.setItemLayout(0, h, h, h);
    layout.setItemLayout(1, spacing, spacing, spacing);
    layout.setItemLayout(2, togWidth, togWidth, togWidth);
    layout.setItemLayout(3, spacing, spacing, spacing);
    layout.setItemLayout(4, h, -1, -1);
    
    Component *comps[] = { icon, nullptr, nullptr, button };
    layout.layOutComponents(comps, 5, b.getX(), b.getY(), b.getWidth(), b.getHeight(), false, true);
}

void PowerFelCategoryItemComponent::buttonClicked(Button *b) {}

void PowerFelCategoryItemComponent::buttonStateChanged(Button *b) {
    //
}

void PowerFelCategoryItemComponent::enablementChanged() {
    updateButtonText();
}

PowerFelPageComponent::PowerFelPageComponent() {
    bgColor = Colour(0xff000000);
    bgImage = "powerMenuBackground.png";
    mainPage = new Component();
    addAndMakeVisible(mainPage);
    mainPage->toBack();
    ChildProcess child{};
    debounce = 0;
    
    // create back button
    backButton = createImageButton(
                                   "Back", createImageFromFile(assetFile("nextIcon.png")));
    backButton->addListener(this);
    backButton->setTriggeredOnMouseDown(true);
    backButton->setAlwaysOnTop(true);
    addAndMakeVisible(backButton);
    
    yesButton = new TextButton("yes");
    yesButton->setButtonText("yes");
    yesButton->addListener(this);
    yesButton->setTriggeredOnMouseDown(true);
    addAndMakeVisible(yesButton);
    
    noButton = new TextButton("no");
    noButton->setButtonText("no");
    noButton->addListener(this);
    noButton->setTriggeredOnMouseDown(true);
    addAndMakeVisible(noButton);

}

PowerFelPageComponent::~PowerFelPageComponent() {}

void PowerFelPageComponent::paint(Graphics &g) {
    g.fillAll(bgColor);
    auto image = createImageFromFile(assetFile(bgImage));
    g.drawImageAt(image,0,0,false);
  
    g.setColour (Colours::white);
    g.setFont (22);
    g.drawText ("about to reboot into software flashing mode.", 0, 40, 480, 40, Justification::centred, true);
    g.drawText ("are you sure?", 0, 60, 480, 40, Justification::centred, true);
}

void PowerFelPageComponent::resized() {
    
    auto bounds = getLocalBounds();
    auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());
  
    {
        for (int i = 0, j = 0; i < 4; ++i) {
            if (i > 0) verticalLayout.setItemLayout(j++, 0, -1, -1);
            verticalLayout.setItemLayout(j++, 48, 48, 48);
        }
        
        Component *PowerFelItems[] = { yesButton.get(), nullptr, nullptr, noButton.get() };
        auto b = bounds.reduced(10);
        b.setLeft(70);
        verticalLayout.layOutComponents(PowerFelItems, 1, b.getX(), b.getY(), b.getWidth(),
                                        b.getHeight(), true, true);
    }
    
    mainPage->setBounds(bounds);
    
    yesButton->setBounds(bounds.getWidth()/3.375, 120, 200, 40);
    noButton->setBounds(bounds.getWidth()/3.375, 185, 200, 40);
    backButton->setBounds(bounds.getWidth()-60, bounds.getY(), 60, bounds.getHeight());
}


void PowerFelPageComponent::buttonClicked(Button *button) {
      if (button == backButton || button == noButton ) {
        getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontalLeft);
      } else if( button == yesButton && !debounce ) {
        debounce = 1;
        DBG( "FEL MODE!");
        child.start("/usr/sbin/gotofel.sh");
      }
  }