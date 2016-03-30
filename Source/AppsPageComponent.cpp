#include "AppsPageComponent.h"
#include "PokeLookAndFeel.h"
#include "Utils.h"

AppIconButton::AppIconButton(const String &label, const String &shell, const Drawable *image)
: DrawableButton(label, DrawableButton::ImageAboveTextLabel),
  shell(shell) {
  setImages(image);
}

Rectangle<float> AppIconButton::getImageBounds() const {
  auto bounds = getLocalBounds();
  return bounds.withHeight(PokeLookAndFeel::getDrawableButtonImageHeightForBounds(bounds)).toFloat();
}

AppsPageComponent::AppsPageComponent() {
  train = new TrainComponent();
  train->itemWidth = 240;
  train->itemHeight = 95;
  train->orientation = TrainComponent::Orientation::kOrientationGrid;
  addAndMakeVisible(train);
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

void AppsPageComponent::buttonClicked(Button *button) {
  auto appButton = (AppIconButton*)button;
  DBG("AppsPageComponent::buttonClicked - shell: " << appButton->shell);
  ChildProcess launchApp{};
  launchApp.start(appButton->shell);
}
