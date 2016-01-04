#include "SwitchComponent.h"

#include "Utils.h"

SwitchComponent::SwitchComponent() {
  handle = new DrawablePath();
  addAndMakeVisible(handle);

  setSize(42, 24);
}

SwitchComponent::~SwitchComponent() {}

void SwitchComponent::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  float radius = float(pillBounds.getHeight()) / 2.0f;

  g.setColour(findColour(colorIdBackground));
  g.fillRoundedRectangle(pillBounds.getX(), pillBounds.getY(), pillBounds.getWidth(),
                         pillBounds.getHeight(), radius);
}

void SwitchComponent::resized() {
  pillBounds.setBounds(0, 0, 15, 10);
  fitRectInRect(pillBounds, getLocalBounds(), Justification::centred, false);
  pillBounds = pillBounds.reduced(2.0);

  {
    auto insetBounds = pillBounds.reduced(4);
    auto d = insetBounds.getHeight();

    handleBoundsOff = { insetBounds.getX(), insetBounds.getY(), d, d };
    handleBoundsOn = { insetBounds.getRight() - d, insetBounds.getY(), d, d };

    Path path;
    path.addEllipse(0, 0, d, d);
    handle->setPath(path);

    handle->setFill(FillType(findColour(colorIdHandle)));
    handle->setBounds(getToggleState() ? handleBoundsOn : handleBoundsOff);
  }
}

void SwitchComponent::clicked() {
  auto bounds = getToggleState() ? handleBoundsOn : handleBoundsOff;
  Desktop::getInstance().getAnimator().animateComponent(handle, bounds, 1.0f, 150, false, 0.0, 0.0);
}
