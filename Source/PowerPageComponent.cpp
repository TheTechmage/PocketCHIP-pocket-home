#include "PowerPageComponent.h"
#include "PowerPageFelComponent.h"
#include "Main.h"
#include "Utils.h"
#include "PokeLookAndFeel.h"

#include <numeric>

void PowerSpinnerTimer::timerCallback() {
    if (powerComponent) {
        auto lsp = powerComponent->powerSpinner.get();
        const auto& lspImg = powerComponent->launchSpinnerImages;
        i++;
        if (i == lspImg.size()) { i = 0; }
      
        lsp->setImage(lspImg[i]);
        
    }
}

PowerPageComponent::PowerPageComponent() {
  bgColor = Colours::black;
  bgImage = createImageFromFile(assetFile("powerMenuBackground.png"));
  mainPage = new Component();
  addAndMakeVisible(mainPage);
  mainPage->toBack();
  ChildProcess child{};
  
  felPage = new PowerFelPageComponent();

  // create back button
  backButton = createImageButton(
      "Back", createImageFromFile(assetFile("nextIcon.png")));
  backButton->addListener(this);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);
    
  powerOffButton = new TextButton("Power OFF");
  powerOffButton->setButtonText("Shutdown");
  powerOffButton->addListener(this);
  addAndMakeVisible(powerOffButton);
    
  rebootButton = new TextButton("Reboot");
  rebootButton->setButtonText("Reboot");
  rebootButton->addListener(this);
  addAndMakeVisible(rebootButton);
    
   sleepButton = new TextButton("Sleep");
   sleepButton->setButtonText("Sleep");
   sleepButton->addListener(this);
   addAndMakeVisible(sleepButton);
    
    felButton = new TextButton("Fel");
    felButton->setButtonText("Flash Software");
    felButton->addListener(this);
    addAndMakeVisible(felButton);
  
    powerSpinnerTimer.powerComponent = this;
    Array<String> spinnerImgPaths{"wait0.png","wait1.png","wait2.png","wait3.png","wait4.png","wait5.png","wait6.png","wait7.png"};
    for(auto& path : spinnerImgPaths) {
        auto image = createImageFromFile(assetFile(path));
        launchSpinnerImages.add(image);
    }
    powerSpinner = new ImageComponent();
    powerSpinner->setImage(launchSpinnerImages[0]);
    addChildComponent(powerSpinner);
  
  buildName = "Build: ";
  auto releaseFileName = absoluteFileFromPath( "/etc/os-release" );
  File releaseFile( releaseFileName );
  if (releaseFile.exists()) {
    auto fileStr = releaseFile.loadFileAsString();
    auto lines = split(fileStr, "\n");
    if (lines.size() < 9)
      DBG(__func__ << ": No release information in /etc/os-release");
    else {
      auto releaseKv = split(lines[8],"=");
      std::vector<String> releaseV(releaseKv.begin()+1,releaseKv.end());
      for (const auto& val : releaseV) {
        // WIP: misses the removed equals
        buildName += val;
      }
      DBG(buildName);
    }
  }
  
#if JUCE_MAC
  buildName = "Build: MacOsX Dev Build";
#endif
  
  buildNameLabel = new Label("Build Name");
  buildNameLabel->setText(buildName, NotificationType::dontSendNotification);
  buildNameLabel->      setFont(16);
  buildNameLabel->setJustificationType(Justification::centred);
  addAndMakeVisible(buildNameLabel);
}

PowerPageComponent::~PowerPageComponent() {}

void PowerPageComponent::paint(Graphics &g) {
    auto bounds = getLocalBounds();
    g.fillAll(bgColor);
    g.drawImage(bgImage,bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, bgImage.getWidth(), bgImage.getHeight(), false);
}

void PowerPageComponent::resized() {
  
  auto bounds = getLocalBounds();
   powerSpinner->setBounds(0, 0, bounds.getWidth(), bounds.getHeight());

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

  powerOffButton->setBounds(bounds.getWidth()/7, 40, 350, 40);
  sleepButton->setBounds(bounds.getWidth()/7, 90, 350, 40);
  rebootButton->setBounds(bounds.getWidth()/7, 140, 350, 40);
  felButton->setBounds(bounds.getWidth()/7, 190, 350, 40);
  backButton->setBounds(bounds.getWidth()-60, bounds.getY(), 60, bounds.getHeight());
  
  buildNameLabel->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), 30);
  buildNameLabel->setBoundsToFit(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), Justification::centredBottom, true);
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

void PowerPageComponent::buttonStateChanged(Button *btn) {
  if (btn->isMouseButtonDown() && btn->isMouseOver()) {
    btn->setAlpha(0.5f);
  }
  else {
    btn->setAlpha(1.0f);
  }
}

void PowerPageComponent::buttonClicked(Button *button) {
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontalLeft);
  } else if (button == powerOffButton) {
    showPowerSpinner();
    child.start("systemctl poweroff");
  } else if (button == rebootButton) {
    showPowerSpinner();
    child.start("systemctl reboot");
  } else if (button == sleepButton) {
    setSleep();
  } else if (button == felButton) {
    getMainStack().pushPage(felPage, PageStackComponent::kTransitionTranslateHorizontalLeft);
  }
}
