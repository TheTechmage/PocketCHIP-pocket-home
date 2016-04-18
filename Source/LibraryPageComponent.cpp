#include "LibraryPageComponent.h"

#include "PokeLookAndFeel.h"
#include "Main.h"
#include "Utils.h"

LibraryPageComponent::LibraryPageComponent() :
  AppListComponent(),
  backButton(createImageButton("Back",
                               createImageFromFile(assetFile("nextIcon.png"))))
{
  bgColor = Colour(PokeLookAndFeel::chipPurple);
  
  backButton->addListener(this);
  backButton->setTriggeredOnMouseDown(true);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);
}
LibraryPageComponent::~LibraryPageComponent() {}

void LibraryPageComponent::paint(Graphics &g) {
  g.fillAll(bgColor);
}

void LibraryPageComponent::resized() {
  AppListComponent::resized();
  
  const auto& b = getLocalBounds();
  auto trainWidth = b.getWidth() - 2*btnHeight;
  auto trainHeight = b.getHeight() - (2.1*btnHeight);
  train->setSize(trainWidth, trainHeight);
  train->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centred, true);
  
  backButton->setBounds(b.getWidth()-60, b.getY(), 60, b.getHeight());
}

void LibraryPageComponent::buttonClicked(Button *button) {
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontalLeft);
  }
  else if (button == prevPageBtn) {
    train->showPrevPage();
    checkShowPageNav();
  }
  else if (button == nextPageBtn) {
    train->showNextPage();
    checkShowPageNav();
  }
  else {
    // TODO: implement downloading here
  }
}