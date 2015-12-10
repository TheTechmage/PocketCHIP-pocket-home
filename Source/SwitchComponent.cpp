#include "SwitchComponent.h"

#include "Utils.h"

SwitchComponent::SwitchComponent() {
  setSize(48, 24);
}

SwitchComponent::~SwitchComponent() {}

void SwitchComponent::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
  auto on = getToggleState();

  float radius = float(pillBounds.getHeight()) / 2.0f;

  g.setColour(Colours::white);
  g.fillRoundedRectangle(pillBounds.getX(), pillBounds.getY(), pillBounds.getWidth(),
                         pillBounds.getHeight(), radius);

  g.setColour(Colours::black);
  g.drawRoundedRectangle(pillBounds.getX(), pillBounds.getY(),
                         pillBounds.getWidth(), pillBounds.getHeight(), radius, 4);

  {
    auto insetBounds = pillBounds.reduced(4);

    float t = on ? 1.0f : 0.0f;
    float d = insetBounds.getHeight();
    float x = mix(insetBounds.getX(), insetBounds.getRight() - d, t);

    g.fillEllipse(x, insetBounds.getY(), d, d);
  }
}

void SwitchComponent::resized() {
  pillBounds.setBounds(0, 0, 2, 1);
  fitRectInRect(pillBounds, getLocalBounds(), Justification::centred, false);
  pillBounds = pillBounds.reduced(2.0);
}
