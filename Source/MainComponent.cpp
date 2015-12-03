#include "MainComponent.h"

static const int categoryButtonHeight = 50;
static const int categoryPadding = 10;

static ScopedPointer<DrawableButton> createCategoryButton(const std::string name,
                                                          const char* svgData) {
  ScopedPointer<DrawableButton> button;
  ScopedPointer<Drawable> icon;
  ScopedPointer<XmlElement> iconSvg (XmlDocument::parse (svgData));

  if (iconSvg != nullptr)
    icon = Drawable::createFromSVG (*iconSvg);

  button = new DrawableButton (name, DrawableButton::ImageFitted);
  button->setImages(icon);
  button->setRadioGroupId(4444);
  button->setClickingTogglesState (true);

  return button;
};

MainContentComponent::MainContentComponent() {
  settingsPage = ScopedPointer<SettingsPageComponent>(new SettingsPageComponent());
  addAndMakeVisible(settingsPage.get());

  appButton = createCategoryButton("Apps", BinaryData::appsIcon_svg);
  addAndMakeVisible(appButton);

  gamesButton = createCategoryButton("Games", BinaryData::gamesIcon_svg);
  addAndMakeVisible(gamesButton);

  settingsButton = createCategoryButton("Settings", BinaryData::settingsIcon_svg);
  addAndMakeVisible(settingsButton);

  categoryButtonLayout.setItemLayout(0, 0, -1.0, -1.0);
  categoryButtonLayout.setItemLayout(1,
                                     categoryButtonHeight+categoryPadding,
                                     categoryButtonHeight+categoryPadding, categoryButtonHeight);
  categoryButtonLayout.setItemLayout(2,
                                     categoryButtonHeight+categoryPadding,
                                     categoryButtonHeight+categoryPadding, categoryButtonHeight);
  categoryButtonLayout.setItemLayout(3,
                                     categoryButtonHeight+categoryPadding,
                                     categoryButtonHeight+categoryPadding, categoryButtonHeight);
  categoryButtonLayout.setItemLayout(4, 0, -1.0, -1.0);

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

  auto bounds = getLocalBounds().reduced(categoryPadding);

  Component *categoryButtons[] = { nullptr, appButton.get(), gamesButton.get(),
                                   settingsButton.get(), nullptr };
  categoryButtonLayout.layOutComponents(categoryButtons, 5, bounds.getX(),
                                        bounds.getY() + categoryPadding,
                                        bounds.getWidth(), categoryButtonHeight, false, true);

}
