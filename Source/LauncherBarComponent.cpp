#include "LauncherBarComponent.h"

static const int launcherButtonHeight = 62;
static const int launcherPadding = 10;

LauncherBarComponent::LauncherBarComponent() {
  int h = launcherButtonHeight;
  int p = launcherPadding;
  launcherButtonLayout.setItemLayout(0, 0, -1.0, -1.0);
  launcherButtonLayout.setItemLayout(1, h + p, h + p, h);
  launcherButtonLayout.setItemLayout(2, h + p, h + p, h);
  launcherButtonLayout.setItemLayout(3, h + p, h + p, h);
  launcherButtonLayout.setItemLayout(4, 0, -1.0, -1.0);
}

LauncherBarComponent::~LauncherBarComponent() {}

void LauncherBarComponent::paint(Graphics &g) {
  g.fillAll(Colours::white); // clear the background

  g.setColour(Colours::grey);
  g.drawRect(getLocalBounds(), 1); // draw an outline around the component

  g.setColour(Colours::lightblue);
  g.setFont(14.0f);
  g.drawText("LauncherBarComponent", getLocalBounds(), Justification::centred,
             true); // draw some placeholder text
}

void LauncherBarComponent::resized() {
  auto bounds = getLocalBounds().reduced(launcherPadding);
}
