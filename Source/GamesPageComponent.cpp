#include "GamesPageComponent.h"

GamesPageComponent::GamesPageComponent() {
  trainIcons.add(new TextButton("Quake"));
  trainIcons.add(new TextButton("Halo"));
  trainIcons.add(new TextButton("Fallout 4"));
  trainIcons.add(new TextButton("Half Life 3"));

  train = new TrainComponent();

  for (auto icon : trainIcons) {
    train->addItem(icon);
  }

  addAndMakeVisible(train);
}

GamesPageComponent::~GamesPageComponent() {}

void GamesPageComponent::paint(Graphics &g) {}

void GamesPageComponent::resized() {
  auto bounds = getLocalBounds();

  train->centreWithSize(bounds.getWidth(), 96);
}
