#include "LauncherComponent.h"
#include "AppsPageComponent.h"
#include "LibraryPageComponent.h"
#include "SettingsPageComponent.h"
#include "PowerPageComponent.h"
#include "PokeLookAndFeel.h"

#include "Main.h"
#include "Utils.h"
#include <math.h>
#include <algorithm>

void LaunchSpinnerTimer::timerCallback() {
  if (launcherComponent) {
    auto lsp = launcherComponent->launchSpinner.get();
    const auto& lspImg = launcherComponent->launchSpinnerImages;
    
    // change image
    i++;
    if (i == lspImg.size()) { i = 0; }
    lsp->setImage(lspImg[i]);
    
    // check timeout
    t += getTimerInterval();
    if (t > timeout) {
      t = 0;
      lsp->setVisible(false);
      stopTimer();
    }
  }
}

void BatteryIconTimer::timerCallback() {
  
  // get current battery status from the battery monitor thread
  auto batteryStatus = launcherComponent->batteryMonitor.getCurrentStatus();
  
  // we can't change anything if we don't have a LauncherComponent
  if(launcherComponent) {
    
    // we want to modify the "Battery" icon
      const auto& batteryIcons = launcherComponent->batteryIconImages;
      const auto& batteryIconsCharging = launcherComponent->batteryIconChargingImages;
      

    for( auto button : launcherComponent->topButtons->buttons ) {
      Image batteryImg = batteryIcons[3];
      if (button->getName() == "Battery") {
          int status = round( ((float)batteryStatus.percentage)/100.0f * 3.0f );
          if( batteryStatus.percentage <= 5 ) {
              status = 3;
          } else {
              // limit status range to [0:3]
              if(status < 0) status = 0;
              if(status > 2) status = 2;
          }
          if( !batteryStatus.isCharging ) {
              batteryImg = batteryIcons[status];
          } else {
              batteryImg = batteryIconsCharging[status];

          }
          
          button->setImages(false, true, true,
                       batteryImg, 1.0f, Colours::transparentWhite, // normal
                       batteryImg, 1.0f, Colours::transparentWhite, // over
                       batteryImg, 0.5f, Colours::transparentWhite, // down
                       0);
      }
    }
  }
  
  //DBG( "Charging: "  << batteryStatus.isCharging );
  //DBG( "Voltage: " << batteryStatus.percentage );
  
}

void WifiIconTimer::timerCallback() {
  if(!launcherComponent) { return; }
    
  for( auto button : launcherComponent->topButtons->buttons ) {
    if (button->getName() == "WiFi") {
      Image wifiIcon;
      const auto& conAp = getWifiStatus().connectedAccessPoint();
      
      // wifi on and connected
      if (getWifiStatus().isConnected() && conAp) {
        // 0 to 100
        float sigStrength = std::max(0, std::min(99, conAp->signalStrength));
        // don't include the wifi-off icon as a bin
        int iconBins = launcherComponent->wifiIconImages.size() - 2;
        int idx = round( ( iconBins * (sigStrength)/100.0f) );
        DBG(__func__ << ": accessing icon " << idx);
        wifiIcon = launcherComponent->wifiIconImages[idx];
      }
      // wifi on but no connection
      else if (getWifiStatus().isEnabled()) {
        wifiIcon = launcherComponent->wifiIconImages[0];
      }
      // wifi off
      else {
        wifiIcon = launcherComponent->wifiIconImages.getLast();
      }
      
      button->setImages(false, true, true,
                        wifiIcon, 1.0f, Colours::transparentWhite, // normal
                        wifiIcon, 1.0f, Colours::transparentWhite, // over
                        wifiIcon, 0.5f, Colours::transparentWhite, // down
                        0);
    }
  }
}

LauncherComponent::LauncherComponent(const var &configJson)
{
  bgColor = Colour(0x000000);
  bgImage = createImageFromFile(assetFile("mainBackground.png"));
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  topButtons = new LauncherBarComponent();
  botButtons = new LauncherBarComponent();
  topButtons->setInterceptsMouseClicks(false, true);
  botButtons->setInterceptsMouseClicks(false, true);
  addAndMakeVisible(topButtons);
  addAndMakeVisible(botButtons);
  
  Array<String> wifiImgPaths{"wifiStrength0.png","wifiStrength1.png","wifiStrength2.png","wifiStrength3.png","wifiOff.png"};
  for(auto& path : wifiImgPaths) {
    auto image = createImageFromFile(assetFile(path));
    wifiIconImages.add(image);
  }
  
  Array<String> batteryImgPaths{"battery_1.png","battery_2.png","battery_3.png","battery_0.png"};
  for(auto& path : batteryImgPaths) {
    auto image = createImageFromFile(assetFile(path));
    batteryIconImages.add(image);
  }
    
  Array<String> batteryImgChargingPaths{"batteryCharging_1.png","batteryCharging_2.png","batteryCharging_3.png","batteryCharging_0.png"};
  for(auto& path : batteryImgChargingPaths) {
    auto image = createImageFromFile(assetFile(path));
    batteryIconChargingImages.add(image);
  }

  launchSpinnerTimer.launcherComponent = this;
  Array<String> spinnerImgPaths{"wait0.png","wait1.png","wait2.png","wait3.png","wait4.png","wait5.png","wait6.png","wait7.png"};
  for(auto& path : spinnerImgPaths) {
    auto image = createImageFromFile(assetFile(path));
    launchSpinnerImages.add(image);
  }
  
  launchSpinner = new ImageComponent();
  launchSpinner->setImage(launchSpinnerImages[0]);
  launchSpinner->setInterceptsMouseClicks(false, false);
  addChildComponent(launchSpinner);
  
  // Settings page
  auto settingsPage = new SettingsPageComponent();
  settingsPage->setName("Settings");
  pages.add(settingsPage);
  pagesByName.set("Settings", settingsPage);
  
  // Power page
  auto powerPage = new PowerPageComponent();
  powerPage->setName("Power");
  pages.add(powerPage);
  pagesByName.set("Power", powerPage);
  
  // Apps page
  auto appsPage = new AppsPageComponent(this);
  appsPage->setName("Apps");
  pages.add(appsPage);
  pagesByName.set("Apps", appsPage);
  
  // AppGet page
  auto appLibrary = new LibraryPageComponent();
  appLibrary->setName("AppLibrary");
  pages.add(appLibrary);
  pagesByName.set("AppLibrary", appLibrary);
  
  // Read config for apps and corner locations
  auto pagesData = configJson["pages"].getArray();
  if (pagesData) {
    for (const auto &page : *pagesData) {
      auto name = page["name"].toString();
      if (name == "Apps") {
        
        // add all items from config to our launch list
        const auto& appButtons = appsPage->createIconsFromJsonArray(page["items"]);
        for (auto button : appButtons) { button->setWantsKeyboardFocus(false); }
        
        // add all items from config to our install list
        auto appsFile = assetFile("ntc-apps.json");
        if (appsFile.exists()) {
          auto appsJson = JSON::parse(appsFile);
          if (appsJson) {
            const auto& appLibBtns = appLibrary->createIconsFromJsonArray(appsJson);
            for (auto button : appLibBtns) { button->setWantsKeyboardFocus(false); }
          }
          else {
            std::cerr << "Could not parse installable applications list: " << appsFile.getFullPathName() << std::endl;
          }
        }
        else {
          std::cerr << "Missing installable applications list: " << appsFile.getFullPathName() << std::endl;
        }
        
        // add corner buttons
        // FIXME: is there a better way to slice juce Array<var> ?
        auto buttonsData = *(page["cornerButtons"].getArray());
        Array<var> topData{};
        Array<var> botData{};
        topData.add(buttonsData[0]);
        topData.add(buttonsData[1]);
        botData.add(buttonsData[2]);
        botData.add(buttonsData[3]);
        
        topButtons->addButtonsFromJsonArray(topData);
        botButtons->addButtonsFromJsonArray(botData);
      }
    }
  }
  
  // NOTE(ryan): Maybe do something with a custom event later.. For now we just listen to all the
  // buttons manually.
  for (auto button : topButtons->buttons) {
    button->setWantsKeyboardFocus(false);
    button->setInterceptsMouseClicks(false, false);
  }
  for (auto button : botButtons->buttons) {
    button->addListener(this);
    button->setWantsKeyboardFocus(false);
  }

  defaultPage = pagesByName[configJson["defaultPage"]];
  
  batteryMonitor.updateStatus();
  batteryMonitor.startThread();
  
  batteryIconTimer.launcherComponent = this;
  batteryIconTimer.startTimer(1000);
  batteryIconTimer.timerCallback();
  
  wifiIconTimer.launcherComponent = this;
  wifiIconTimer.startTimer(2000);
  wifiIconTimer.timerCallback();
}

LauncherComponent::~LauncherComponent() {
  batteryIconTimer.stopTimer();
  batteryMonitor.stopThread(2000);
}

void LauncherComponent::paint(Graphics &g) {
  auto bounds = getLocalBounds();
  g.fillAll(bgColor);
  g.drawImage(bgImage,bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, bgImage.getWidth(), bgImage.getHeight(), false);
}

void LauncherComponent::resized() {
  auto bounds = getLocalBounds();
  int barSize = PokeLookAndFeel::getButtonHeight();
  
  topButtons->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(),
                        barSize);
  botButtons->setBounds(bounds.getX(), bounds.getHeight() - barSize, bounds.getWidth(),
                             barSize);
  pageStack->setBounds(bounds.getX() + barSize, bounds.getY(), bounds.getWidth() - 2*barSize,
                       bounds.getHeight());
  launchSpinner->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());

  // init
  if (!resize) {
    resize = true;
    pageStack->swapPage(defaultPage, PageStackComponent::kTransitionNone);
  }
}

void LauncherComponent::showLaunchSpinner() {
  DBG("Show launch spinner");
  launchSpinner->setVisible(true);
  launchSpinnerTimer.startTimer(500);
}

void LauncherComponent::hideLaunchSpinner() {
  DBG("Hide launch spinner");
  launchSpinnerTimer.stopTimer();
  launchSpinner->setVisible(false);
}

void LauncherComponent::openAppLibrary() {
  getMainStack().pushPage(pagesByName["AppLibrary"], PageStackComponent::kTransitionTranslateHorizontalLeft);
}

void LauncherComponent::buttonClicked(Button *button) {
  auto currentPage = pageStack->getCurrentPage();
  if ((!currentPage || currentPage->getName() != button->getName()) &&
      pagesByName.contains(button->getName())) {
    auto page = pagesByName[button->getName()];
    if (button->getName() == "Settings") {
      getMainStack().pushPage(page, PageStackComponent::kTransitionTranslateHorizontal);
    } else if (button->getName() == "Power") {
        getMainStack().pushPage(page, PageStackComponent::kTransitionTranslateHorizontalLeft);
    } else {
      pageStack->swapPage(page, PageStackComponent::kTransitionTranslateHorizontal);
    }
  }
}
