#include "MainComponent.h"



MainContentComponent::MainContentComponent() {
  settingsPage = std::unique_ptr<SettingsPageComponent>(new SettingsPageComponent());
  addAndMakeVisible(settingsPage.get());

  ScopedPointer<XmlElement> appsIconSvg (XmlDocument::parse (BinaryData::appsIcon_svg));
  ScopedPointer<XmlElement> gamesIconSvg (XmlDocument::parse (BinaryData::gamesIcon_svg));
  ScopedPointer<XmlElement> settingsIconSvg (XmlDocument::parse (BinaryData::settingsIcon_svg));

  if (appsIconSvg != nullptr)
    appsIcon = Drawable::createFromSVG (*appsIconSvg);

  if (gamesIconSvg != nullptr)
    gamesIcon = Drawable::createFromSVG (*gamesIconSvg);

  if (settingsIconSvg != nullptr)
    settingsIcon = Drawable::createFromSVG (*settingsIconSvg);

  appsIcon->setBounds(0, 0, 100, 100);
  gamesIcon->setBounds(200, 0, 100, 100);
  settingsIcon->setBounds(400, 0, 100, 100);

  addAndMakeVisible(appsIcon);
  addAndMakeVisible(gamesIcon);
  addAndMakeVisible(settingsIcon);

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
