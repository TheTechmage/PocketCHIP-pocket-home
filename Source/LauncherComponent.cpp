#include "LauncherComponent.h"
#include "AppsPageComponent.h"
#include "SettingsPageComponent.h"
#include "Main.h"
#include "Utils.h"

LauncherComponent::LauncherComponent(const var &configJson) {
  bgColor = Colour(0xff2e8dbd);
  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  topButtons = new LauncherBarComponent();
  botButtons = new LauncherBarComponent();
  addAndMakeVisible(botButtons);
  addAndMakeVisible(topButtons);

  auto pagesData = configJson["pages"].getArray();
  if (pagesData) {
    for (const auto &page : *pagesData) {
      auto name = page["name"].toString();
      Component *pageComponent = nullptr;
      if (name == "Settings") {
        pageComponent = new SettingsPageComponent();
      } else {
        auto appsPage = new AppsPageComponent();
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

    // NOTE(ryan): Maybe do something with a custom event later.. For now we just listen to all the
    // buttons manually.
    for (auto button : topButtons->buttons) {
      button->addListener(this);
    }
    for (auto button : botButtons->buttons) {
      button->addListener(this);
      button->setTriggeredOnMouseDown(true);
    }
  }

  defaultPage = pagesByName[configJson["defaultPage"]];
}

LauncherComponent::~LauncherComponent() {}

void LauncherComponent::paint(Graphics &g) {
  g.fillAll(bgColor);
}

void LauncherComponent::resized() {
  auto bounds = getLocalBounds();
  int barSize = 54;
  
  botButtons->setBounds(bounds.getX(), bounds.getHeight() - barSize, bounds.getWidth(),
                             barSize);
  topButtons->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(),
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
