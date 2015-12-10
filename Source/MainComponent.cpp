#include "MainComponent.h"
#include "PokeLookAndFeel.h"
#include "Utils.h"

MainContentComponent::MainContentComponent(const var &configJson) {
  lookAndFeel = new PokeLookAndFeel();
  setLookAndFeel(lookAndFeel);

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
      pageStack->addChildComponent(page);
    }

    categoryButtons->addCategoriesFromJsonArray(*categories);
    categoryButtons->setInterceptsMouseClicks(false, true);

    // NOTE(ryan): Maybe do something with a custom event later.. For now we just listen to all the
    // buttons manually.
    for (auto button : categoryButtons->buttons) {
      button->addListener(this);
    }
  }

  closeButton = new TextButton("Close");
  addAndMakeVisible(closeButton);
  closeButton->addListener(this);

  setSize(480, 245);
}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::paint(Graphics &g) {
  g.fillAll(Colours::white);
}

void MainContentComponent::resized() {
  auto bounds = getLocalBounds();
  closeButton->setBounds(bounds.getWidth() - 40, 0, 40, 20);
  categoryButtons->setBounds(bounds.getX(), bounds.getY() + 10, bounds.getWidth(),
                             categoryButtons->buttonSize);
  pageStack->setBounds(bounds);
  for (auto page : pages) {
    page->setBounds(bounds);
  }
}

void MainContentComponent::buttonClicked(Button *button) {
  auto currentPage = pageStack->getCurrentPage();
  if ((!currentPage || currentPage->getName() != button->getName()) &&
      pagesByName.contains(button->getName())) {
    pageStack->swapPage(pagesByName[button->getName()],
                        PageStackComponent::kTransitionTranslateHorizontal);
  }
  if (button == closeButton) {
    JUCEApplication::quit();
  }
}
