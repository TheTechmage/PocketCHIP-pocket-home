#include "IconSliderComponent.h"

static ScopedPointer<DrawableButton> createIconButton(const std::string name, const char *svgData) {
  ScopedPointer<DrawableButton> button;
  ScopedPointer<Drawable> icon;
  ScopedPointer<XmlElement> iconSvg(XmlDocument::parse(svgData));

  if (iconSvg != nullptr) icon = Drawable::createFromSVG(*iconSvg);

  button = new DrawableButton(name, DrawableButton::ImageFitted);
  button->setImages(icon);
  button->setRadioGroupId(4444);
  button->setClickingTogglesState(true);
  button->setColour(DrawableButton::backgroundOnColourId, Colour(0xffffffff));

  return button;
};

IconSliderComponent::IconSliderComponent(const char *iconLoData, const char *iconHiData) {
  auto createSlider = [&] {
    auto s = new Slider();
    s->setSliderStyle(Slider::LinearHorizontal);
    s->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    return ScopedPointer<Slider>(s);
  };

  iconLow = createIconButton("Lo", iconLoData);
  iconHi = createIconButton("Hi", iconHiData);

  addAndMakeVisible(iconLow);
  addAndMakeVisible(iconHi);

  slider = createSlider();
  addAndMakeVisible(slider);

  sliderLayout.setItemLayout(0, 0.0, -1.0, -1.0);
  sliderLayout.setItemLayout(1, 50, -1.0, -1.0);
  sliderLayout.setItemLayout(2, 0.0, -1.0, -1.0);
}

IconSliderComponent::~IconSliderComponent() {}

void IconSliderComponent::paint(Graphics &g) {}

void IconSliderComponent::resized() {
  auto bounds = getLocalBounds();

  sliderLayout.setItemLayout(0, bounds.getHeight(), bounds.getHeight(), bounds.getHeight());
  sliderLayout.setItemLayout(2, bounds.getHeight(), bounds.getHeight(), bounds.getHeight());

  Component *parts[] = { iconLow.get(), slider.get(), iconHi.get() };
  sliderLayout.layOutComponents(parts, 3, bounds.getX(), bounds.getY(), bounds.getWidth(),
                                bounds.getHeight(), false, true);
}
