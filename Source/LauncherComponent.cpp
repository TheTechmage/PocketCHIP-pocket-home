#include "LauncherComponent.h"
#include "AppsPageComponent.h"
#include "SettingsPageComponent.h"
#include "Main.h"
#include "Utils.h"

LauncherComponent::LauncherComponent(const var &configJson) {
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  categoryButtons = new LauncherBarComponent(62);
  addAndMakeVisible(categoryButtons);

  auto categories = configJson.getArray();
  if (categories) {
    for (const auto &category : *categories) {
      auto name = category["name"].toString();
      auto page = name == "Settings" ? new SettingsPageComponent() : new AppsPageComponent();
      page->setName(name);
      page->createIconsFromJsonArray(category["items"]);
      pages.add(page);
      pagesByName.set(name, page);
    }

    categoryButtons->addCategoriesFromJsonArray(*categories);
    categoryButtons->setInterceptsMouseClicks(false, true);

    // NOTE(ryan): Maybe do something with a custom event later.. For now we just listen to all the
    // buttons manually.
    for (auto button : categoryButtons->buttons) {
      button->addListener(this);
    }
  }
}

LauncherComponent::~LauncherComponent() {}

void LauncherComponent::paint(Graphics &g) {
  g.fillAll(Colours::white);
}

void LauncherComponent::resized() {
  auto bounds = getLocalBounds();
  categoryButtons->setBounds(bounds.getX(), bounds.getHeight() - categoryButtons->buttonSize,
                             bounds.getWidth(), categoryButtons->buttonSize);
//  pageStack->setBounds(bounds);
  pageStack->setBounds(bounds.getX(), bounds.getY(),
                       bounds.getWidth(), bounds.getHeight() - categoryButtons->buttonSize);
  for (auto page : pages) {
    page->setBounds(bounds);
  }
}

void LauncherComponent::buttonClicked(Button *button) {
  auto currentPage = pageStack->getCurrentPage();
  if ((!currentPage || currentPage->getName() != button->getName()) &&
      pagesByName.contains(button->getName())) {
    auto page = pagesByName[button->getName()];
    if (button->getName() == "Settings") {
      getMainStack().pushPage(page, PageStackComponent::kTransitionTranslateHorizontal);
    } else {
      pageStack->swapPage(page, PageStackComponent::kTransitionTranslateHorizontal);
    }
  }
}
