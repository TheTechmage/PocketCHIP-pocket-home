#include "AppsPageComponent.h"
#include "LauncherComponent.h"
#include "PokeLookAndFeel.h"
#include "Utils.h"

void AppCheckTimer::timerCallback() {
  DBG("AppCheckTimer::timerCallback - check running apps");
  if (appsPage) {
    appsPage->checkRunningApps();
  }
}

void AppDebounceTimer::timerCallback() {
  DBG("AppDebounceTimer::timerCallback - check launch debounce");
  if (appsPage) {
    appsPage->debounce = false;
  }
  stopTimer();
}

AppIconButton::AppIconButton(const String &label, const String &shell, const Drawable *image)
: DrawableButton(label, DrawableButton::ImageAboveTextLabel),
  shell(shell) {
  // FIXME: supposedly setImages will "create internal copies of its drawables"
  // this relates to AppsPageComponent ownership of drawable icons ... docs are unclear
  setImages(image);
}

Rectangle<float> AppIconButton::getImageBounds() const {
  auto bounds = getLocalBounds();
  return bounds.withHeight(PokeLookAndFeel::getDrawableButtonImageHeightForBounds(bounds)).toFloat();
}

AppsPageComponent::AppsPageComponent(LauncherComponent* launcherComponent) :
  train(new TrainComponent()),
  launcherComponent(launcherComponent),
  runningCheckTimer(),
  debounceTimer()
{
  // WIP: rip em, these don't matter for grid
  train->itemWidth = 186;
  train->itemHeight = 109;
  train->orientation = TrainComponent::Orientation::kOrientationGrid;
  addAndMakeVisible(train);
  
  runningCheckTimer.appsPage = this;
  debounceTimer.appsPage = this;
}

AppsPageComponent::~AppsPageComponent() {}

void AppsPageComponent::paint(Graphics &g) {}

void AppsPageComponent::resized() {
  auto bounds = getLocalBounds();
  train->centreWithSize(bounds.getWidth(), bounds.getHeight());
}

void AppsPageComponent::addAndOwnIcon(const String &name, Component *icon) {
  trainIcons.add(icon);
  train->addItem(icon);
  ((Button*)icon)->setTriggeredOnMouseDown(true);
  ((Button*)icon)->addListener(this);
}

DrawableButton *AppsPageComponent::createAndOwnIcon(const String &name, const String &shell, const String &iconPath) {
  auto image = createImageFromFile(assetFile(iconPath));
  auto drawable = new DrawableImage();
  drawable->setImage(image);
  // FIXME: is this OwnedArray for the drawables actually necessary?
  // won't the AppIconButton correctly own the drawable?
  // Further we don't actually use this list anywhere.
  iconDrawableImages.add(drawable);
  auto button = new AppIconButton(name, shell, drawable);
  addAndOwnIcon(name, button);
  return button;
}

Array<DrawableButton *> AppsPageComponent::createIconsFromJsonArray(const var &json) {
  Array<DrawableButton *> buttons;
  if (json.isArray()) {
    for (const auto &item : *json.getArray()) {
      auto name = item["name"];
      auto shell = item["shell"];
      auto iconPath = item["icon"];
      if (name.isString() && shell.isString() && iconPath.isString()) {
        auto icon = createAndOwnIcon(name, shell, iconPath);
        if (icon) {
          buttons.add(icon);
        }
      }
    }
  }
  return buttons;
}

void AppsPageComponent::startApp(AppIconButton* appButton) {
  auto launchApp = new ChildProcess();
  if (launchApp->start(appButton->shell)) {
    runningApps.add(launchApp);
    runningAppsByButton.set(appButton, runningApps.indexOf(launchApp));
    runningCheckTimer.startTimer(5 * 1000);
    
    debounce = true;
    debounceTimer.startTimer(2 * 1000);
    
    // TODO: should probably put app button clicking logic up into LauncherComponent
    // correct level for event handling needs more thought
    launcherComponent->showLaunchSpinner();
  }
};

void AppsPageComponent::focusApp(AppIconButton* appButton) {
  DBG("focusApp: IMPLEMENT ME");
};

void AppsPageComponent::checkRunningApps() {
  Array<int> needsRemove{};
  
  // check list to mark any needing removal
  for (const auto& cp : runningApps) {
    if (!cp->isRunning()) {
      needsRemove.add(runningApps.indexOf(cp));
    }
  }
  
  // cleanup list
  for (const auto appIdx : needsRemove) {
    runningApps.remove(appIdx);
    runningAppsByButton.removeValue(appIdx);
  }
  
  if (!runningApps.size()) {
    runningCheckTimer.stopTimer();
    launcherComponent->hideLaunchSpinner();
  }
};

void AppsPageComponent::buttonClicked(Button *button) {
  auto appButton = (AppIconButton*)button;
  
  // FIXME: debounce and running apps need to integrate, once running check works
  if (debounce) {
    focusApp(appButton);
    DBG("AppsPageComponent::buttonClicked - switch focus: " << appButton->shell);
  }
  else {
    startApp(appButton);
    DBG("AppsPageComponent::buttonClicked - shell: " << appButton->shell);
  }
}
