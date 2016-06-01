#include "AppsPageComponent.h"
#include "LauncherComponent.h"
#include "PokeLookAndFeel.h"
#include "Main.h"
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

AppListComponent::AppListComponent() :
  grid(new Grid(3, 2)),
  nextPageBtn(createImageButton("NextAppsPage",
                                ImageFileFormat::loadFrom(assetFile("pageDownIcon.png")))),
  prevPageBtn(createImageButton("PrevAppsPage",
                                ImageFileFormat::loadFrom(assetFile("pageUpIcon.png"))))
{
  addChildComponent(nextPageBtn);
  addChildComponent(prevPageBtn);
  nextPageBtn->addListener(this);
  prevPageBtn->addListener(this);
  
  addAndMakeVisible(grid);
}
AppListComponent::~AppListComponent() {}

AppIconButton* AppListComponent::createAndOwnIcon(const String &name, const String &iconPath, const String &shell) {
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

void AppListComponent::resized() {
  auto b = getLocalBounds();
  int btnHeight = PokeLookAndFeel::getButtonHeight();
  
  prevPageBtn->setSize(btnHeight, btnHeight);
  nextPageBtn->setSize(btnHeight, btnHeight);
  prevPageBtn->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centredTop, true);
  nextPageBtn->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centredBottom, true);
  
  // drop the page buttons from our available layout size
  auto gridWidth = b.getWidth();
  auto gridHeight = b.getHeight() - (2.0*btnHeight);
  grid->setSize(gridWidth, gridHeight);
  grid->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centred, true);
}

void AppListComponent::checkShowPageNav() {
  if (grid->hasNextPage()) {
    nextPageBtn->setVisible(true); nextPageBtn->setEnabled(true);
  }
  else {
    nextPageBtn->setVisible(false); nextPageBtn->setEnabled(false);
  }
  
  if (grid->hasPrevPage()) {
    prevPageBtn->setVisible(true); prevPageBtn->setEnabled(true);
  }
  else {
    prevPageBtn->setVisible(false); prevPageBtn->setEnabled(false);
  }
}

// FIXME: these button handlers not being scopeable to specific button types or instances
// is pretty annoying. It means we have to check instances and types at runtime to see what
// button is changing. See also buttonClicked()
void AppListComponent::buttonStateChanged(Button* btn) {
  auto appBtn = dynamic_cast<AppIconButton*>(btn);
  if (!appBtn) { return; }
  
  auto appIcon = static_cast<DrawableImage*>(appBtn->getCurrentImage());
  auto& buttonPopup = getMainButtonPopup();
  auto& parentComp = getMainContentComponent();
  
  constexpr auto scale = 1.3;
  
  // show floating button popup if we're holding downstate and not showing the popup
  if (btn->isMouseButtonDown() &&
      btn->isMouseOver() &&
      !buttonPopup.isVisible()) {
    // copy application icon bounds in screen space
    auto boundsNext = appIcon->getScreenBounds();
    auto boundsCentre = boundsNext.getCentre();
    
    // scale and recenter
    boundsNext.setSize(boundsNext.getWidth()*scale, boundsNext.getHeight()*scale);
    boundsNext.setCentre(boundsCentre);
    
    // translate back to space local to popup parent (local bounds)
    auto parentPos = parentComp.getScreenPosition();
    boundsNext.setPosition(boundsNext.getPosition() - parentPos);
    
    // show popup icon, hide real button beneath
    buttonPopup.setImage(appIcon->getImage());
    buttonPopup.setBounds(boundsNext);
    buttonPopup.setVisible(true);
    appIcon->setVisible(false);
    // set icon text dim
    auto dimWhite = Colour(0x99ffffff);
    appBtn->setColour(DrawableButton::textColourId, dimWhite);
  }
  // set UI back to default if we can see the popup, but aren't holding the button down
  else if (btn->isVisible()) {
    appIcon->setVisible(true);
    appBtn->setColour(DrawableButton::textColourId, getLookAndFeel().findColour(DrawableButton::textColourId));
    buttonPopup.setVisible(false);
  }
}

void AppListComponent::addAndOwnIcon(const String &name, Component *icon) {
  gridIcons.add(icon);
  grid->addItem(icon);
  ((Button*)icon)->addListener(this);
}

Array<AppIconButton*> AppListComponent::createIconsFromJsonArray(const var &json) {
  Array<AppIconButton*> buttons;
  if (json.isArray()) {
    for (const auto &item : *json.getArray()) {
      auto name = item["name"];
      auto shell = item["shell"];
      auto iconPath = item["icon"];
      if (name.isString() && shell.isString() && iconPath.isString()) {
        auto icon = createAndOwnIcon(name, iconPath, shell);
        if (icon) {
          buttons.add(icon);
        }
      }
    }
  }
  
  checkShowPageNav();
  return buttons;
}

AppsPageComponent::AppsPageComponent(LauncherComponent* launcherComponent) :
  AppListComponent(),
  launcherComponent(launcherComponent),
  runningCheckTimer(),
  debounceTimer()
{
  runningCheckTimer.appsPage = this;
  debounceTimer.appsPage = this;
}

AppsPageComponent::~AppsPageComponent() {}

Array<AppIconButton*> AppsPageComponent::createIconsFromJsonArray(const var &json) {
  auto buttons = AppListComponent::createIconsFromJsonArray(json);
  
  // hard coded "virtual" application. Cannot be removed.
  appLibraryBtn = createAndOwnIcon("App Get", "appIcons/install.png", String::empty);
  buttons.add(appLibraryBtn);
  checkShowPageNav();
  
  return buttons;
}

void AppsPageComponent::startApp(AppIconButton* appButton) {
  DBG("AppsPageComponent::startApp - " << appButton->shell);
  auto launchApp = new ChildProcess();
  launchApp->start("xmodmap ${HOME}/.Xmodmap"); // Reload xmodmap to ensure it's running
  if (launchApp->start(appButton->shell)) {
    runningApps.add(launchApp);
    runningAppsByButton.set(appButton, runningApps.indexOf(launchApp));
    // FIXME: uncomment when process running check works
    // runningCheckTimer.startTimer(5 * 1000);
    
    debounce = true;
    debounceTimer.startTimer(2 * 1000);
    
    // TODO: should probably put app button clicking logic up into LauncherComponent
    // correct level for event handling needs more thought
    launcherComponent->showLaunchSpinner();
  }
};

void AppsPageComponent::focusApp(AppIconButton* appButton, const String& windowId) {
  DBG("AppsPageComponent::focusApp - " << appButton->shell);
  String focusShell = "echo 'focus_client_by_window_id("+windowId+")' | awesome-client";
  StringArray focusCmd{"sh", "-c", focusShell.toRawUTF8()};
  ChildProcess focusWindow;
  focusWindow.start(focusCmd);
};

void AppsPageComponent::startOrFocusApp(AppIconButton* appButton) {
  if (debounce) return;
  
  bool shouldStart = true;
  int appIdx = runningAppsByButton[appButton];
  bool hasLaunched = runningApps[appIdx] != nullptr;
  String windowId;
  
  if(hasLaunched) {
    const auto shellWords = split(appButton->shell, " ");
    const auto& cmdName = shellWords[0];
    StringArray findCmd{"xdotool", "search", "--all", "--limit", "1", "--class", cmdName.toRawUTF8()};
    ChildProcess findWindow;
    findWindow.start(findCmd);
    findWindow.waitForProcessToFinish(1000);
    windowId = findWindow.readAllProcessOutput().trimEnd();
    
    // does xdotool find a window id? if so, we shouldn't start a new one
    shouldStart = (windowId.length() > 0) ? false : true;
  }
  
  if (shouldStart) {
    startApp(appButton);
  }
  else {
    focusApp(appButton, windowId);
  }
  
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
    // FIXME: uncomment when process running check works
    // runningCheckTimer.stopTimer();
    launcherComponent->hideLaunchSpinner();
  }
};

void AppsPageComponent::buttonClicked(Button *button) {
  if (button == prevPageBtn) {
    grid->showPrevPage();
    checkShowPageNav();
  }
  else if (button == nextPageBtn) {
    grid->showNextPage();
    checkShowPageNav();
  }
  else if (button == appLibraryBtn) {
    launcherComponent->openAppLibrary();
  }
  else {
    auto appButton = (AppIconButton*)button;
    startOrFocusApp(appButton);
  }
}
