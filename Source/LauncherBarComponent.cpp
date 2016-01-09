#include "LauncherBarComponent.h"
#include "Utils.h"

LauncherBarButton::LauncherBarButton(const String &name, const Image &image) : ImageButton(name) {
  setImages(true, true, true,                       //
            image, 1.0f, Colours::transparentBlack, // normal
            image, 1.0f, Colours::transparentBlack, // over
            image, 1.0f, Colours::transparentBlack, // down
            0);
}

void LauncherBarButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto bgBounds = Rectangle<int>(0, 0, 1, 1);
  fitRectInRect(bgBounds, getLocalBounds(), Justification::centred, false);

  g.setColour(findColour(TextButton::buttonColourId));
  g.fillEllipse(bgBounds.toFloat());

  ImageButton::paintButton(g, isMouseOverButton, isButtonDown);
}

LauncherBarComponent::LauncherBarComponent() {
  tempIcon = Drawable::createFromImageData(BinaryData::appsIcon_png, BinaryData::appsIcon_pngSize);
}

LauncherBarComponent::~LauncherBarComponent() {}

void LauncherBarComponent::paint(Graphics &g) {}

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
  bounds.reduce(buttonPadding / 2, buttonPadding);

  int buttonSize = bounds.getHeight();

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
  auto image = createImageFromFile(iconFile);
  auto button = new LauncherBarButton(name, image);
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
