#include "AppsPageComponent.h"

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

void AppsPageComponent::populateIconsWithJsonArray(const var &json) {
  if (json.isArray()) {
    for (const auto &item : *json.getArray()) {
      auto name = item["name"];
      // auto icon = item["icon"];

      if (name.isString()) {
        auto button = new TextButton(name);
        trainIcons.add(button);
        train->addItem(button);
      }
    }
  }
}
