#include "../JuceLibraryCode/JuceHeader.h"
#include "AppsPageComponent.h"

AppsPageComponent::AppsPageComponent() {
  trainIcons.add(new TextButton("Iceweasel"));
  trainIcons.add(new TextButton("MPlayer"));
  trainIcons.add(new TextButton("Terminal"));
  trainIcons.add(new TextButton("Viewnior"));

  train = new TrainComponent();

  for (auto icon : trainIcons) {
    train->addItem(icon);
  }

  addAndMakeVisible(train);
}

AppsPageComponent::~AppsPageComponent() {}

void AppsPageComponent::paint(Graphics &g) {}

void AppsPageComponent::resized() {
  auto bounds = getLocalBounds();

  train->centreWithSize(bounds.getWidth(), 100);
}
