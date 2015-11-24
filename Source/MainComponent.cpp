#include "MainComponent.h"

MainContentComponent::MainContentComponent() {
  setSize(480, 272);
}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::paint(Graphics &g) {
  g.fillAll(Colour(0xff202020));

  g.setFont(Font(16.0f));
  g.setColour(Colours::white);
  g.drawText("Herro PokeCHIP", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized() {}
