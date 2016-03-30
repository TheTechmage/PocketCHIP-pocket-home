#include "LauncherComponent.h"
#include "AppsPageComponent.h"
#include "SettingsPageComponent.h"
#include "Main.h"
#include "Utils.h"

LauncherComponent::LauncherComponent(const var &configJson) {
  bgColor = Colour(0xff2e8dbd);
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  categoryButtons = new LauncherBarComponent();
  addAndMakeVisible(categoryButtons);

  auto categories = configJson["categories"].getArray();
  if (categories) {
    for (const auto &category : *categories) {
      auto name = category["name"].toString();
      Component *page = nullptr;
      if (name == "Settings") {
        page = new SettingsPageComponent();
      } else {
        auto appsPage = new AppsPageComponent();
        appsPage->createIconsFromJsonArray(category["items"]);
        page = appsPage;
      }
      page->setName(name);
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

  defaultPage = pagesByName[configJson["defaultCategory"]];
}

LauncherComponent::~LauncherComponent() {}

void LauncherComponent::paint(Graphics &g) {
  g.fillAll(bgColor);
}

void LauncherComponent::resized() {
  auto bounds = getLocalBounds();
  int barSize = 54;
  
  categoryButtons->setBounds(bounds.getX(), bounds.getHeight() - barSize, bounds.getWidth(),
                             barSize);
  pageStack->setBounds(bounds.getX() + barSize, bounds.getY() + barSize/2, bounds.getWidth() - 2*barSize,
                       bounds.getHeight() - barSize);

  // init
  if (!resize) {
    resize = true;
    pageStack->swapPage(defaultPage, PageStackComponent::kTransitionNone);
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
