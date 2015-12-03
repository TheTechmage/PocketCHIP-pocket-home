#include "MainComponent.h"



MainContentComponent::MainContentComponent() {
  settingsPage = std::unique_ptr<SettingsPageComponent>(new SettingsPageComponent());
  addAndMakeVisible(settingsPage.get());

//    void createCategoryButton();

  ScopedPointer<XmlElement> appsIconSvg (XmlDocument::parse (BinaryData::appsIcon_svg));
  ScopedPointer<XmlElement> gamesIconSvg (XmlDocument::parse (BinaryData::gamesIcon_svg));
  ScopedPointer<XmlElement> settingsIconSvg (XmlDocument::parse (BinaryData::settingsIcon_svg));

  if (appsIconSvg != nullptr)
    appsIcon = Drawable::createFromSVG (*appsIconSvg);

  if (gamesIconSvg != nullptr)
    gamesIcon = Drawable::createFromSVG (*gamesIconSvg);

  if (settingsIconSvg != nullptr)
    settingsIcon = Drawable::createFromSVG (*settingsIconSvg);

  appButton = new DrawableButton ("Apps", DrawableButton::ImageFitted);
  appButton->setImages(appsIcon);
  appButton->setRadioGroupId(4444);
  appButton->setBounds (140, 30, 50, 50);
  appButton->setClickingTogglesState (true);
  addAndMakeVisible(appButton);

  gamesButton = new DrawableButton ("Games", DrawableButton::ImageFitted);
  gamesButton->setImages(gamesIcon);
  gamesButton->setRadioGroupId(4444);
  gamesButton->setBounds (200, 30, 50, 50);
  gamesButton->setClickingTogglesState (true);
  addAndMakeVisible(gamesButton);

  settingsButton = new DrawableButton ("Settings", DrawableButton::ImageFitted);
  settingsButton->setImages(settingsIcon);
  settingsButton->setRadioGroupId(4444);
  settingsButton->setBounds(260, 30, 50, 50);
  settingsButton->setClickingTogglesState (true);
  addAndMakeVisible(settingsButton);


  setSize(480, 245);
}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::paint(Graphics &g) {
  g.fillAll(Colour(0xff202020));

  g.setFont(Font(16.0f));
  g.setColour(Colours::white);
  g.drawText("Herro PokeCHIP", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized() {
  settingsPage->setBounds(getLocalBounds());
}
