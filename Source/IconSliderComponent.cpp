#include "IconSliderComponent.h"
#include "Utils.h"

IconSliderComponent::IconSliderComponent(const Drawable &iconLoDrawable,
                                         const Drawable &iconHiDrawable) {
  auto createSlider = [&] {
    auto s = new Slider();
    s->setSliderStyle(Slider::LinearHorizontal);
    s->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    return ScopedPointer<Slider>(s);
  };

  iconLow = new DrawableButton("lo", DrawableButton::ImageFitted);
  iconLow->setImages(&iconLoDrawable);

  iconHi = new DrawableButton("hi", DrawableButton::ImageFitted);
  iconHi->setImages(&iconHiDrawable);

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
