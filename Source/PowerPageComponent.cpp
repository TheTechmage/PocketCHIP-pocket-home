#include "PowerPageComponent.h"
#include "Main.h"
#include "Utils.h"
#include "PokeLookAndFeel.h"

#include <numeric>

void PowerDebounceTimer::timerCallback() {
    DBG("APowerDebounceTimer::timerCallback - check power debounce");
    if (powerComponent) {
        powerComponent->debounce = false;
    }
    stopTimer();
}

void PowerSpinnerTimer::timerCallback() {
    if (powerComponent) {
        auto lsp = powerComponent->powerSpinner.get();
        const auto& lspImg = powerComponent->launchSpinnerImages;
        
        i++;
        if (i == lspImg.size()) { i = 0; }
        lsp->setImage(lspImg[i]);
    }
}

PowerCategoryButton::PowerCategoryButton(const String &name)
: Button(name),
  displayText(name)
{}

void PowerCategoryButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
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

void PowerCategoryButton::resized() {
  pillBounds.setSize(getLocalBounds().getWidth(), 42);
  fitRectInRect(pillBounds, getLocalBounds(), Justification::centred, false);
}

void PowerCategoryButton::setText(const String &text) {
  displayText = text;
  repaint();
}

PowerCategoryItemComponent::PowerCategoryItemComponent(const String &name)
: icon{ new DrawableButton("icon", DrawableButton::ImageFitted) },
  button{ new PowerCategoryButton(name) } {
  addAndMakeVisible(icon);
  addAndMakeVisible(button);
  button->setEnabled(false); // default to disabled state
}

void PowerCategoryItemComponent::paint(Graphics &g) {}

void PowerCategoryItemComponent::resized() {
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

void PowerCategoryItemComponent::buttonClicked(Button *b) {}

void PowerCategoryItemComponent::buttonStateChanged(Button *b) {
    //
}

void PowerCategoryItemComponent::enablementChanged() {
  updateButtonText();
}

PowerPageComponent::PowerPageComponent() {
  bgColor = Colour(0xff000000);
  bgImage = "powerMenuBackground.png";
  mainPage = new Component();
  addAndMakeVisible(mainPage);
  mainPage->toBack();
  ChildProcess child{};
  debounce = 0;

  // create back button
  backButton = createImageButton(
      "Back", createImageFromFile(assetFile("backIcon.png")));
  backButton->addListener(this);
  backButton->setTriggeredOnMouseDown(true);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);
    
  powerOffButton = new TextButton("Power OFF");
  powerOffButton->setButtonText("shutdown");
  powerOffButton->addListener(this);
  powerOffButton->setTriggeredOnMouseDown(true);
  addAndMakeVisible(powerOffButton);
    
  rebootButton = new TextButton("Reboot");
  rebootButton->setButtonText("reboot");
  rebootButton->addListener(this);
  rebootButton->setTriggeredOnMouseDown(true);
  addAndMakeVisible(rebootButton);
    
   sleepButton = new TextButton("Sleep");
   sleepButton->setButtonText("sleep");
   sleepButton->addListener(this);
   sleepButton->setTriggeredOnMouseDown(true);
   addAndMakeVisible(sleepButton);
    
    felButton = new TextButton("Fel");
    felButton->setButtonText("flash software");
    felButton->addListener(this);
    felButton->setTriggeredOnMouseDown(true);
    addAndMakeVisible(felButton);

    powerDebounceTimer.powerComponent = this;
    
    powerSpinnerTimer.powerComponent = this;
    Array<String> spinnerImgPaths{"wait1.png","wait2.png","wait3.png","wait4.png"};
    for(auto& path : spinnerImgPaths) {
        auto image = createImageFromFile(assetFile(path));
        launchSpinnerImages.add(image);
    }
    powerSpinner = new ImageComponent();
    powerSpinner->setImage(launchSpinnerImages[0]);
    addChildComponent(powerSpinner);
}

PowerPageComponent::~PowerPageComponent() {}

void PowerPageComponent::paint(Graphics &g) {
    g.fillAll(bgColor);
    auto image = createImageFromFile(assetFile(bgImage));
    g.drawImageAt(image,0,0,false);
}

void PowerPageComponent::resized() {
  
  auto bounds = getLocalBounds();
   powerSpinner->setBounds(bounds.getWidth()/3., 0, bounds.getWidth()/3., bounds.getHeight());

  {
    for (int i = 0, j = 0; i < 4; ++i) {
      if (i > 0) verticalLayout.setItemLayout(j++, 0, -1, -1);
      verticalLayout.setItemLayout(j++, 48, 48, 48);
    }

    Component *powerItems[] = { powerOffButton.get(), rebootButton.get(), sleepButton.get() };
    auto b = bounds.reduced(10);
    b.setLeft(70);
    verticalLayout.layOutComponents(powerItems, 1, b.getX(), b.getY(), b.getWidth(),
                                    b.getHeight(), true, true);
  }

  mainPage->setBounds(bounds);

  powerOffButton->setBounds(bounds.getWidth()/3.375, 40, 200, 40);
  sleepButton->setBounds(bounds.getWidth()/3.375, 90, 200, 40);
  rebootButton->setBounds(bounds.getWidth()/3.375, 140, 200, 40);
  felButton->setBounds(bounds.getWidth()/3.375, 190, 200, 40);
  backButton->setBounds(bounds.getX(), bounds.getY(), 60, bounds.getHeight());
}

void PowerPageComponent::setSleep() {
    #if JUCE_LINUX
    StringArray cmd{ "xset","q","|","grep","is O" };
    if(child.start(cmd)) {
        const String result (child.readAllProcessOutput());
        if( result == "Monitor is Off") {
            child.start("xset dpms force on");
        } else {
            child.start("xset dpms force off" );
        }
    }
  #endif
}

void PowerPageComponent::showPowerSpinner() {
    backButton->setVisible(false);
    powerOffButton->setVisible(false);
    sleepButton->setVisible(false);
    rebootButton->setVisible(false);
    felButton->setVisible(false);
    powerSpinner->setVisible(true);
    powerSpinnerTimer.startTimer(1*1000);
}

void PowerPageComponent::buttonClicked(Button *button) {
  if( !debounce ) {
    if (button == backButton) {
      getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontal);
    } else if (button == powerOffButton) {
      debounce = true;
      showPowerSpinner();
      child.start("systemctl poweroff");
    } else if (button == rebootButton) {
      debounce = true;
      showPowerSpinner();
      child.start("systemctl reboot");
    } else if (button == sleepButton) {
      debounce = true;
      powerDebounceTimer.startTimer(2 * 1000);
      setSleep();
    } else if (button == felButton) {
      debounce = true;
      powerDebounceTimer.startTimer(2 * 1000);
      child.start("felmode");
    }
  }
}
