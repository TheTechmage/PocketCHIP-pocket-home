#include "MainComponent.h"
#include "PokeLookAndFeel.h"

MainContentComponent::MainContentComponent() {
  lookAndFeel = new PokeLookAndFeel();
  setLookAndFeel(lookAndFeel);

  auto configJson = JSON::parse(R"json(
[
  {
    "name": "Apps",
    "icon": "../../../../assets/appsIcon.svg",
    "items": [
      {
        "name": "Terminal",
        "icon": "../../../../assets/Faenza/utilities-terminal.png"
      },
      {
        "name": "Iceweasel",
        "icon": "../../../../assets/Faenza/iceweasel.png"
      },
      {
        "name": "MPlayer",
        "icon": "../../../../assets/Faenza/mplayer.png"
      },
      {
        "name": "MyPaint",
        "icon": "../../../../assets/Faenza/mypaint.png"
      }
    ]
  },
  {
    "name": "Games",
    "icon": "../../../../assets/appsIcon.svg",
    "items": [
      {
        "name": "Quake",
        "icon": "../../../../assets/Faenza/window.png"
      }
    ]
  }
]
  )json");

  auto categories = configJson.getArray();
  if (categories) {
    for (const auto &category : *categories) {
      auto name = category["name"].toString();
      auto page = new AppsPageComponent();
      page->addIconsFromJsonArray(category["items"]);
      pages.add(page);
      pagesByName.set(name, page);
      addChildComponent(page);
    }

    auto settingsPage = new SettingsPageComponent();
    pages.add(settingsPage);
    pagesByName.set("Settings", settingsPage);
    addChildComponent(settingsPage);

    categoryButtons = new LauncherBarComponent(62);
    categoryButtons->addCategoriesFromJsonArray(*categories);
    categoryButtons->addCategory("Settings");
    addAndMakeVisible(categoryButtons);

    // NOTE(ryan): Maybe do something with a custom event later.. For now we just listen to all the
    // buttons manually.
    for (auto button : categoryButtons->buttons) {
      button->addListener(this);
    }
  }

  closeButton = new TextButton("Close");
  closeButton->setBounds(0, 0, 40, 20);
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

  categoryButtons->setBounds(bounds.getX(), bounds.getY() + 10, bounds.getWidth(), categoryButtons->buttonSize);

  for (auto page : pages) {
    page->setBounds(bounds);
  }
}

void MainContentComponent::buttonClicked(Button *button) {
  for (auto page : pages) {
    page->setVisible(false);
  }
  if (pagesByName.contains(button->getName())) {
    pagesByName[button->getName()]->setVisible(true);
  }
}
