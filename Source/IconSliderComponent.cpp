#include "IconSliderComponent.h"
#include "Utils.h"
#include "SettingsPageComponent.h"
#include "PokeLookAndFeel.h"

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
  slider->setRange(0,100);
  slider->addListener(this);
}

IconSliderComponent::~IconSliderComponent() {}

void IconSliderComponent::paint(Graphics &g) {
}

void IconSliderComponent::addListener(SettingsPageComponent *page) {
  listener = page;
}


void IconSliderComponent::sliderDragStarted(Slider* slider) {
  listener->sliderDragStarted(this);
}

void IconSliderComponent::sliderDragEnded(Slider* slider) {
  listener->sliderDragEnded(this);
}

void IconSliderComponent::sliderValueChanged(Slider* slider) {
  listener->sliderValueChanged(this);
}

void IconSliderComponent::resized() {
  auto bounds = getLocalBounds();
  auto bh = bounds.getHeight();
  auto bw = bounds.getWidth();
  auto btnHeight = PokeLookAndFeel::getButtonHeight();

  sliderLayout.setItemLayout(0, bh, bh, bh);
  sliderLayout.setItemLayout(1, btnHeight, -1.0, -1.0);
  sliderLayout.setItemLayout(2, bh, bh, bh);

  Component *parts[] = { iconLow, slider, iconHi };
  sliderLayout.layOutComponents(parts, 3, bounds.getX(), bounds.getY(), bw, bh, false, true);
}
