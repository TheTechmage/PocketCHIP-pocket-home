#include "IconSliderComponent.h"

static ScopedPointer<DrawableButton> createIconButton(const std::string name, const char *svgData) {
  ScopedPointer<DrawableButton> button;
  ScopedPointer<Drawable> icon;
  ScopedPointer<XmlElement> iconSvg(XmlDocument::parse(svgData));

  if (iconSvg != nullptr) icon = Drawable::createFromSVG(*iconSvg);

  button = new DrawableButton(name, DrawableButton::ImageFitted);
  button->setImages(icon);

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
}

IconSliderComponent::~IconSliderComponent() {}

void IconSliderComponent::paint(Graphics &g) {
}

void IconSliderComponent::resized() {
  auto bounds = getLocalBounds();
  auto bh = bounds.getHeight();
  auto bw = bounds.getWidth();

  sliderLayout.setItemLayout(0, bh, bh, bh);
  sliderLayout.setItemLayout(1, 50, -1.0, -1.0);
  sliderLayout.setItemLayout(2, bh, bh, bh);

  Component *parts[] = { iconLow, slider, iconHi };
  sliderLayout.layOutComponents(parts, 3, bounds.getX(), bounds.getY(), bw, bh, false, true);
}
