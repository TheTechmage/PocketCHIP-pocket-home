#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "LauncherBarComponent.h"
#include "PageStackComponent.h"
#include "BatteryMonitor.h"

class LauncherComponent;
class AppsPageComponent;

class LaunchSpinnerTimer : public Timer {
public:
    LaunchSpinnerTimer() {};
    void timerCallback();
  
    LauncherComponent* launcherComponent;
  
    int i = 0;
    int t = 0;
    int timeout = 30 * 1000;
};

class BatteryIconTimer : public Timer {
public:
    BatteryIconTimer() {};
    void timerCallback();
    LauncherComponent* launcherComponent;
};

class WifiIconTimer : public Timer {
public:
  WifiIconTimer() {};
  void timerCallback();
  LauncherComponent* launcherComponent;
};

class LauncherComponent : public Component, private Button::Listener {
public:
    BatteryMonitor batteryMonitor;
    ScopedPointer<LauncherBarComponent> botButtons;
    ScopedPointer<LauncherBarComponent> topButtons;
    ScopedPointer<ImageComponent> launchSpinner;
  
    Array<Image> launchSpinnerImages;
    Array<Image> batteryIconImages;
    Array<Image> batteryIconChargingImages;
    Array<Image> wifiIconImages;
  
    LaunchSpinnerTimer launchSpinnerTimer;
    BatteryIconTimer batteryIconTimer;
    WifiIconTimer wifiIconTimer;
    Component* defaultPage;
  
    // FIXME: we have no need for the pages/pagesByName if we're using scoped pointers for each page.
    // All these variables do is add an extra string key the compiler can't see through.
    OwnedArray<Component> pages;
    ScopedPointer<PageStackComponent> pageStack;
    HashMap<String, Component *> pagesByName;
    
    bool resize = false;
    
    StretchableLayoutManager categoryButtonLayout;
    
    LauncherComponent(const var &configJson);
    ~LauncherComponent();
    
    void paint(Graphics &) override;
    void resized() override;
  
    void showLaunchSpinner();
    void hideLaunchSpinner();
    void openAppLibrary();
  
private:
    Colour bgColor;
    Image bgImage;
  
    void buttonClicked(Button *) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LauncherComponent)
};