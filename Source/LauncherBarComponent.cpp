#include "LauncherBarComponent.h"
#include "Utils.h"

LauncherBarButton::LauncherBarButton(const String &name, const Image &image) : ImageButton(name) {
  setImages(true, true, true,                       //
            image, 1.0f, Colours::transparentWhite, // normal
            image, 1.0f, Colours::transparentWhite, // over
            image, 1.0f, Colours::transparentWhite, // down
            0);
}

void LauncherBarButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  //auto bgBounds = Rectangle<int>(0, 0, 1, 1);
  //fitRectInRect(bgBounds, getLocalBounds(), Justification::centred, false);

  //g.setColour(findColour(TextButton::buttonColourId));
  //g.fillEllipse(bgBounds.toFloat());

  ImageButton::paintButton(g, isMouseOverButton, isButtonDown);
}

// FIXME: this should be renamed e.g. CornerButtonsComponent
LauncherBarComponent::LauncherBarComponent() {
  tempIcon = Drawable::createFromImageData(BinaryData::appsIcon_png, BinaryData::appsIcon_pngSize);
}

LauncherBarComponent::~LauncherBarComponent() {}

void LauncherBarComponent::paint(Graphics &g) {}

void LauncherBarComponent::resized() {
  // +1 to add room for spacer
  int nitems = buttons.size() + 1;
  Component *items[nitems];

  {
    int i = 0;
    for (auto button : buttons) {
      items[i++] = button;
    }
  }
  
  // insert the spacer second to last
  items[nitems-1] = items[nitems-2];
  items[nitems-2] = nullptr;

  auto bounds = getLocalBounds();
  bounds.reduce(buttonPadding / 2, buttonPadding);
  int buttonSize = bounds.getHeight();

  if (layoutDirty) {
    int itemWidth = buttonSize + buttonPadding;
    for (int i = 0; i < nitems; i++) {
      layout.setItemLayout(i, itemWidth, itemWidth, buttonSize);
    }
    layout.setItemLayout(nitems - 2, 0, -1.0, -1.0); // set second to last item (spacer) to be 100%
  }

  layout.layOutComponents(items, nitems, bounds.getX(), bounds.getY(), bounds.getWidth(),
                          bounds.getHeight(), false, true);
}

void LauncherBarComponent::buttonClicked(Button *button) {}

void LauncherBarComponent::addButton(const String &name, const String &iconPath) {
  auto iconFile = assetFile(iconPath);
  auto image = createImageFromFile(iconFile);
  auto button = new LauncherBarButton(name, image);
  button->addListener(this);
  buttons.add(button);
  addAndMakeVisible(button);
  layoutDirty = true;
}

// FIXME: this component now only accepts two corner buttons.
// should have an add function which explicitly accepts each.
// even better would be to modify this object to fill the background and accept four corners.
void LauncherBarComponent::addButtonsFromJsonArray(const Array<var> &buttons) {
  for (const auto &button : buttons) {
    auto name = button["name"].toString();
    auto icon = button["icon"].toString();
    addButton(name, icon);
  }
}
