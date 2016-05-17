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
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);
}
LibraryPageComponent::~LibraryPageComponent() {}

void LibraryPageComponent::paint(Graphics &g) {
  g.fillAll(bgColor);
}

void LibraryPageComponent::resized() {
  AppListComponent::resized();
  auto btnHeight = PokeLookAndFeel::getButtonHeight();
  
  const auto& b = getLocalBounds();
  auto gridWidth = b.getWidth() - 2*btnHeight;
  auto gridHeight = b.getHeight() - (2.1*btnHeight);
  grid->setSize(gridWidth, gridHeight);
  grid->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centred, true);
  
  backButton->setBounds(b.getWidth()-btnHeight, b.getY(), btnHeight, b.getHeight());
}

void LibraryPageComponent::buttonClicked(Button *button) {
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontalLeft);
  }
  else if (button == prevPageBtn) {
    grid->showPrevPage();
    checkShowPageNav();
  }
  else if (button == nextPageBtn) {
    grid->showNextPage();
    checkShowPageNav();
  }
  else {
    // TODO: implement downloading here
  }
}