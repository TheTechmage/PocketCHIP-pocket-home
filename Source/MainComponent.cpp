#include "MainComponent.h"
#include "PokeLookAndFeel.h"
#include "LauncherComponent.h"
#include "Utils.h"

MainContentComponent::MainContentComponent(const var &configJson) {
  lookAndFeel = new PokeLookAndFeel();
  setLookAndFeel(lookAndFeel);

  pageStack = new PageStackComponent();
  addAndMakeVisible(pageStack);

  launcher = new LauncherComponent(configJson);
  pageStack->pushPage(launcher, PageStackComponent::kTransitionNone);

  closeButton = new TextButton("Close");
  addAndMakeVisible(closeButton);
  closeButton->addListener(this);

  setSize(480, 225);
}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::paint(Graphics &g) {
  g.fillAll(Colours::white);
}

void MainContentComponent::resized() {
  auto bounds = getLocalBounds();
  pageStack->setBounds(bounds);
  closeButton->setBounds(bounds.getRight() - 40, 0, 40, 20);
}

void MainContentComponent::buttonClicked(Button *button) {
  if (button == closeButton) {
    JUCEApplication::quit();
  }
}
