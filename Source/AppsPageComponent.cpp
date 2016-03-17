#include "AppsPageComponent.h"
#include "PokeLookAndFeel.h"
#include "Utils.h"

AppIconButton::AppIconButton(const String &label, const Drawable *image)
: DrawableButton(label, DrawableButton::ImageAboveTextLabel) {
  setImages(image);
}

Rectangle<float> AppIconButton::getImageBounds() const {
  auto bounds = getLocalBounds();
  return bounds.withHeight(PokeLookAndFeel::getDrawableButtonImageHeightForBounds(bounds)).toFloat();
}

AppsPageComponent::AppsPageComponent() {
  train = new TrainComponent();
  train->itemWidth = 110;
  addAndMakeVisible(train);
}

AppsPageComponent::~AppsPageComponent() {}

void AppsPageComponent::paint(Graphics &g) {}

void AppsPageComponent::resized() {
  auto bounds = getLocalBounds();
  train->centreWithSize(bounds.getWidth(), 120);
}

void AppsPageComponent::addAndOwnIcon(const String &name, Component *icon) {
  trainIcons.add(icon);
  train->addItem(icon);
}

DrawableButton *AppsPageComponent::createAndOwnIcon(const String &name, const String &iconPath) {
  auto image = createImageFromFile(absoluteFileFromPath(iconPath));
  auto drawable = new DrawableImage();
  drawable->setImage(image);
  iconDrawableImages.add(drawable);
  auto button = new AppIconButton(name, drawable);
  addAndOwnIcon(name, button);
  return button;
}

Array<DrawableButton *> AppsPageComponent::createIconsFromJsonArray(const var &json) {
  Array<DrawableButton *> buttons;
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
