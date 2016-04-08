#include "LauncherComponent.h"
#include "AppsPageComponent.h"
#include "SettingsPageComponent.h"
#include "Main.h"
#include "Utils.h"
#include <math.h>

void LaunchSpinnerTimer::timerCallback() {
  if (launcherComponent) {
    auto lsp = launcherComponent->launchSpinner.get();
    const auto& lspImg = launcherComponent->launchSpinnerImages;
    
    i++;
    if (i == lspImg.size()) { i = 0; }
    lsp->setImage(lspImg[i]);
  }
}

void BatteryIconTimer::timerCallback() {
  
  // get current battery status from the battery monitor thread
  auto batteryStatus = launcherComponent->batteryMonitor.getCurrentStatus();
  
  // we can't change anything if we don't have a LauncherComponent
  if(launcherComponent) {
    
    // we want to modify the "Battery" icon
    for( auto button : launcherComponent->topButtons->buttons ) {
      if (button->getName() == "Battery") {
        
        int status = round( ((float)batteryStatus.percentage)/100.0f * 3.0f );
        
        // limit status range to [0:3]
        if(status < 0) status = 0;
        if(status > 3) status = 3;
        
        const auto& batteryIcons = launcherComponent->batteryIconImages;
        button->setImages(true, true, true,                       //
                       batteryIcons[status], 1.0f, Colours::transparentWhite, // normal
                       batteryIcons[status], 1.0f, Colours::transparentWhite, // over
                       batteryIcons[status], 1.0f, Colours::transparentWhite, // down
                       0);
      }
    }
  }
  
  //DBG( "Charging: "  << batteryStatus.isCharging );
  //DBG( "Voltage: " << batteryStatus.percentage );
  
}

LauncherComponent::LauncherComponent(const var &configJson) {
  bgColor = Colour(0xff2e8dbd);
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  topButtons = new LauncherBarComponent();
  botButtons = new LauncherBarComponent();
  addAndMakeVisible(topButtons);
  addAndMakeVisible(botButtons);
  
  batteryMonitor.startThread();
  
  batteryIconTimer.launcherComponent = this;
  batteryIconTimer.startTimer(1000);
  
  Array<String> batteryImgPaths{"battery_0.png","battery_1.png","battery_2.png","battery_3.png"};
  for(auto& path : batteryImgPaths) {
    auto image = createImageFromFile(assetFile(path));
    batteryIconImages.add(image);
  }
  
  launchSpinnerTimer.launcherComponent = this;
  Array<String> spinnerImgPaths{"wait1.png","wait2.png","wait3.png","wait4.png"};
  for(auto& path : spinnerImgPaths) {
    auto image = createImageFromFile(assetFile(path));
    launchSpinnerImages.add(image);
  }
  launchSpinner = new ImageComponent();
  launchSpinner->setImage(launchSpinnerImages[0]);
  addChildComponent(launchSpinner);

  auto pagesData = configJson["pages"].getArray();
  if (pagesData) {
    for (const auto &page : *pagesData) {
      auto name = page["name"].toString();
      Component *pageComponent = nullptr;
      if (name == "Settings") {
        pageComponent = new SettingsPageComponent();
      } else if( name == "Power") {
          pageComponent = new SettingsPageComponent();
      } else if( name == "WiFi") {
          pageComponent = new SettingsPageComponent();
      } else if( name == "Battery") {
          pageComponent = new SettingsPageComponent();
      } else {
        auto appsPage = new AppsPageComponent(this);
        appsPage->createIconsFromJsonArray(page["items"]);
        auto buttonsData = *(page["cornerButtons"].getArray());
        
        // FIXME: is there a better way to slice juce Array<var> ?
        Array<var> topData{};
        Array<var> botData{};
        topData.add(buttonsData[0]);
        topData.add(buttonsData[1]);
        botData.add(buttonsData[2]);
        botData.add(buttonsData[3]);
        
        topButtons->addButtonsFromJsonArray(topData);
        botButtons->addButtonsFromJsonArray(botData);

        pageComponent = appsPage;
      }
      pageComponent->setName(name);
      pages.add(pageComponent);
      pagesByName.set(name, pageComponent);
    }

    topButtons->setInterceptsMouseClicks(false, true);
    botButtons->setInterceptsMouseClicks(false, true);
    launchSpinner->setInterceptsMouseClicks(true, false);

    // NOTE(ryan): Maybe do something with a custom event later.. For now we just listen to all the
    // buttons manually.
    for (auto button : topButtons->buttons) {
      button->addListener(this);
      button->setTriggeredOnMouseDown(true);
    }
    for (auto button : botButtons->buttons) {
      button->addListener(this);
      button->setTriggeredOnMouseDown(true);
    }
  }

  defaultPage = pagesByName[configJson["defaultPage"]];
}

LauncherComponent::~LauncherComponent() {
  batteryIconTimer.stopTimer();
  batteryMonitor.stopThread(2000);
}

void LauncherComponent::paint(Graphics &g) {
  g.fillAll(bgColor);
}

void LauncherComponent::resized() {
  auto bounds = getLocalBounds();
  int barSize = 50;
  
  topButtons->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(),
                        barSize);
  botButtons->setBounds(bounds.getX(), bounds.getHeight() - barSize, bounds.getWidth(),
                             barSize);
  pageStack->setBounds(bounds.getX() + barSize, bounds.getY() + barSize/2, bounds.getWidth() - 2*barSize,
                       bounds.getHeight() - barSize);
  launchSpinner->setBounds(bounds.getWidth()/3., 0, bounds.getWidth()/3., bounds.getHeight());

  // init
  if (!resize) {
    resize = true;
    pageStack->swapPage(defaultPage, PageStackComponent::kTransitionNone);
  }
}

void LauncherComponent::showLaunchSpinner() {
  DBG("Show launch spinner");
  launchSpinner->setVisible(true);
  launchSpinnerTimer.startTimer(1*1000);
}

void LauncherComponent::hideLaunchSpinner() {
  DBG("Hide launch spinner");
  launchSpinnerTimer.stopTimer();
  launchSpinner->setVisible(false);
}

void LauncherComponent::buttonClicked(Button *button) {
  auto currentPage = pageStack->getCurrentPage();
  if ((!currentPage || currentPage->getName() != button->getName()) &&
      pagesByName.contains(button->getName())) {
    auto page = pagesByName[button->getName()];
    if (button->getName() == "Settings" || button->getName() == "WiFi" ) {
      getMainStack().pushPage(page, PageStackComponent::kTransitionTranslateHorizontal);
    } else if (button->getName() == "Power" || button->getName() == "Battery" ) {
        DBG("LauncherComponent::buttonClicked - power");
        ChildProcess child{};
        child.start("chip-exit");
    } else {
      pageStack->swapPage(page, PageStackComponent::kTransitionTranslateHorizontal);
    }
  }
}
