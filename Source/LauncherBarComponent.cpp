#include "LauncherBarComponent.h"
#include "Utils.h"

LauncherBarComponent::LauncherBarComponent(int buttonSize) : buttonSize{ buttonSize } {
  tempIcon = Drawable::createFromImageData(BinaryData::appsIcon_png, BinaryData::appsIcon_pngSize);
}

LauncherBarComponent::~LauncherBarComponent() {}

void LauncherBarComponent::paint(Graphics &g) {
  g.fillAll (Colours::black); // clear the background
}

void LauncherBarComponent::resized() {

  int nitems = buttons.size() + 1;
  Component *items[nitems];
  items[nitems - 2] = nullptr; // second to last item is spacer

  int i = 0;
  for (auto button : buttons) {
    if (button->getName() == "Settings") {
      items[nitems - 1] = button; // set last button to be settings
      continue;
    }
    items[i++] = button;
  }

  auto bounds = getLocalBounds();

  if (layoutDirty) {
    int itemWidth = buttonSize + buttonPadding;
    int i = 0;
    for (int j = 0; j < nitems; ++j) {
      layout.setItemLayout(i++, itemWidth, itemWidth, buttonSize);
    }
    layout.setItemLayout(nitems - 2, 0, -1.0, -1.0); // set second to last item (spacer) to be 100%
  }

  layout.layOutComponents(items, nitems, bounds.getX(), bounds.getY(), bounds.getWidth(),
                          bounds.getHeight(), false, true);
}

void LauncherBarComponent::buttonClicked(Button *button) {}

void LauncherBarComponent::addCategory(const String &name, const String &iconPath) {
  auto iconFile = File(absoluteFileFromPath(iconPath));
  auto button = createImageButton(name, iconFile);
  button->addListener(this);
  buttons.add(button);
  addAndMakeVisible(button);
  layoutDirty = true;
}

void LauncherBarComponent::addCategoriesFromJsonArray(const Array<var> &categories) {
  for (const auto &category : categories) {
    auto name = category["name"].toString();
    auto icon = category["icon"].toString();
    addCategory(name, icon);
  }
}
