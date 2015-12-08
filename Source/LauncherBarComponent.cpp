#include "LauncherBarComponent.h"

LauncherBarComponent::LauncherBarComponent(int buttonSize) : buttonSize{ buttonSize } {
  ScopedPointer<XmlElement> iconSvg = XmlDocument::parse(BinaryData::appsIcon_svg);
  ScopedPointer<XmlElement> iconSelSvg = XmlDocument::parse(BinaryData::appsIconSel_svg);

  tempIcon = Drawable::createFromSVG(*iconSvg);
  tempIconSelected = Drawable::createFromSVG(*iconSelSvg);
}

LauncherBarComponent::~LauncherBarComponent() {}

void LauncherBarComponent::paint(Graphics &g) {}

void LauncherBarComponent::resized() {
  int nitems = buttons.size() + 2;
  Component *items[nitems];
  items[0] = nullptr;
  items[nitems - 1] = nullptr;

  int i = 1;
  for (auto button : buttons) {
    items[i++] = button;
  }

  auto bounds = getLocalBounds();

  if (layoutDirty) {
    int itemWidth = buttonSize + buttonPadding;
    int i = 0;
    layout.setItemLayout(i++, 0, -1.0, -1.0);
    for (int j = 0; j < buttons.size(); ++j) {
      layout.setItemLayout(i++, itemWidth, itemWidth, buttonSize);
    }
    layout.setItemLayout(i, 0, -1.0, -1.0);
  }

  layout.layOutComponents(items, nitems, bounds.getX(), bounds.getY(), bounds.getWidth(),
                          bounds.getHeight(), false, true);
}

void LauncherBarComponent::buttonClicked(Button *button) {}

void LauncherBarComponent::addCategory(const String &name) {
  auto button = new DrawableButton(name, DrawableButton::ImageFitted);
  button->setImages(tempIcon, nullptr, nullptr, nullptr, tempIconSelected);
  button->setRadioGroupId(4444);
  button->setClickingTogglesState(true);
  button->setColour(DrawableButton::backgroundOnColourId, Colour(0xffffffff));
  // button->addListener(this);

  buttons.add(button);
  addAndMakeVisible(button);

  layoutDirty = true;
}

void LauncherBarComponent::addCategoriesFromJsonArray(const Array<var> &categories) {
  for (const auto &category : categories) {
    auto name = category["name"].toString();
    addCategory(name);
  }
}
