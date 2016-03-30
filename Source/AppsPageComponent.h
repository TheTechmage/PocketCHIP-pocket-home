#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "TrainComponent.h"

class AppsPageComponent;

class AppCheckTimer : public Timer {
public:
  AppCheckTimer() {};
  virtual void timerCallback() override;
  // FIXME: better pointer usage, weakref for cycle relationship?
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
  ScopedPointer<TrainComponent> train;
  OwnedArray<Component> trainIcons;
  OwnedArray<DrawableImage> iconDrawableImages;
  OwnedArray<ChildProcess> runningApps;
  
  using AppRunningMap = HashMap<AppIconButton*, int>;

  AppsPageComponent();
  ~AppsPageComponent();

  void paint(Graphics &) override;
  void resized() override;
  void checkRunningApps();

  void addAndOwnIcon(const String &name, Component *icon);
  DrawableButton *createAndOwnIcon(const String &name, const String &shell, const String &iconPath);
  Array<DrawableButton *> createIconsFromJsonArray(const var &json);

  void buttonClicked(Button *) override;

private:
  void startApp(AppIconButton* appButton);
  void focusApp(AppIconButton* appButton);
  
  AppRunningMap runningAppsByButton;
  AppCheckTimer runningCheckTimer;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppsPageComponent)
};
