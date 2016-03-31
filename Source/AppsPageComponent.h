#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TrainComponent.h"

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

class AppsPageComponent : public Component, public Button::Listener {
public:
  AppsPageComponent(LauncherComponent* launcherComponent);
  ~AppsPageComponent();
  
  ScopedPointer<TrainComponent> train;
  OwnedArray<Component> trainIcons;
  OwnedArray<DrawableImage> iconDrawableImages;
  OwnedArray<ChildProcess> runningApps;
  
  bool debounce = false;
  
  using AppRunningMap = HashMap<AppIconButton*, int>;

  void paint(Graphics &) override;
  void resized() override;
  void checkRunningApps();

  void addAndOwnIcon(const String &name, Component *icon);
  DrawableButton *createAndOwnIcon(const String &name, const String &shell, const String &iconPath);
  Array<DrawableButton *> createIconsFromJsonArray(const var &json);

  void buttonClicked(Button *) override;

private:
  LauncherComponent* launcherComponent;
  
  AppRunningMap runningAppsByButton;
  AppCheckTimer runningCheckTimer;
  AppDebounceTimer debounceTimer;
  
  void startApp(AppIconButton* appButton);
  void focusApp(AppIconButton* appButton);
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppsPageComponent)
};
