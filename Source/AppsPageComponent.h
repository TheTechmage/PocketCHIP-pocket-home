#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Grid.h"

class AppsPageComponent;
class LauncherComponent;

class AppCheckTimer : public Timer {
public:
  AppCheckTimer() {};
  virtual void timerCallback() override;
  // TODO: better pointer usage, weakref for cycle relationship?
  AppsPageComponent* appsPage;
};

class AppDebounceTimer : public Timer {
public:
  AppDebounceTimer() {};
  virtual void timerCallback() override;
  // TODO: better pointer usage, weakref for cycle relationship?
  AppsPageComponent* appsPage;
};

class AppIconButton : public DrawableButton {
public:
  AppIconButton(const String &label, const String &shell, const Drawable *image);
  
  const String shell;
  
  Rectangle<float> getImageBounds() const override;
};

class AppListComponent : public Component, public Button::Listener {
public:
  AppListComponent();
  ~AppListComponent();
  
  ScopedPointer<Grid> grid;
  
  OwnedArray<Component> gridIcons;
  OwnedArray<DrawableImage> iconDrawableImages;
  ScopedPointer<ImageButton> nextPageBtn;
  ScopedPointer<ImageButton> prevPageBtn;

  void resized() override;
  void checkShowPageNav();
  
  void addAndOwnIcon(const String &name, Component *icon);
  AppIconButton* createAndOwnIcon(const String &name, const String &iconPath, const String &shell);
  virtual Array<AppIconButton*> createIconsFromJsonArray(const var &json);
  
  void buttonStateChanged(Button* btn) override;
  void buttonClicked(Button* btn) override {};
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppListComponent)
};

class AppsPageComponent : public AppListComponent {
public:
  AppsPageComponent(LauncherComponent* launcherComponent);
  ~AppsPageComponent();
  
  Array<AppIconButton*> createIconsFromJsonArray(const var &json) override;
  
  OwnedArray<ChildProcess> runningApps;
  
  void buttonClicked(Button *button) override;
  
  void checkRunningApps();
  
  bool debounce = false;

private:
  using AppRunningMap = HashMap<AppIconButton*, int>;

  LauncherComponent* launcherComponent;
  AppIconButton* appLibraryBtn;
  
  AppRunningMap runningAppsByButton;
  AppCheckTimer runningCheckTimer;
  AppDebounceTimer debounceTimer;

  void startApp(AppIconButton* appButton);
  void focusApp(AppIconButton* appButton, const String& windowId);
  void startOrFocusApp(AppIconButton* appButton);
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppsPageComponent)
};
