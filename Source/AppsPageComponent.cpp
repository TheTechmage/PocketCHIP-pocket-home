#include "AppsPageComponent.h"
#include "Utils.h"

AppsPageComponent::AppsPageComponent() {
  train = new TrainComponent();
  addAndMakeVisible(train);
}

AppsPageComponent::~AppsPageComponent() {}

void AppsPageComponent::paint(Graphics &g) {}

void AppsPageComponent::resized() {
  auto bounds = getLocalBounds();
  train->centreWithSize(bounds.getWidth(), 96);
}

void AppsPageComponent::addAndOwnIcon(const String &name, Component *icon) {
  trainIcons.add(icon);
  train->addItem(icon);
}

ImageButton *AppsPageComponent::createAndOwnIcon(const String &name, const String &iconPath) {
  auto imageButton = createImageButton(name, absoluteFileFromPath(iconPath));
  if (imageButton) {
    addAndOwnIcon(name, imageButton);
  } else {
    std::cerr << "Could not load icon from " << iconPath << std::endl;
  }
  return imageButton;
}

Array<ImageButton *> AppsPageComponent::createIconsFromJsonArray(const var &json) {
  Array<ImageButton *> buttons;
  if (json.isArray()) {
    for (const auto &item : *json.getArray()) {
      auto name = item["name"];
      auto iconPath = item["icon"];
      if (name.isString() && iconPath.isString()) {
        auto icon = createAndOwnIcon(name, iconPath);
        if (icon) {
          buttons.add(icon);
        }
      }
    }
  }
  return buttons;
}

void AppsPageComponent::buttonClicked(Button *button) {}
