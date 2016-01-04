#include "PokeLookAndFeel.h"
#include "SwitchComponent.h"
#include "Utils.h"

PokeLookAndFeel::PokeLookAndFeel() {
  auto lightGrey = Colour(0xffe1e1e1);
  auto chipPink = Colour(0xffeb008b);

  setColour(Slider::backgroundColourId, lightGrey);
  setColour(Slider::thumbColourId, chipPink);
  setColour(TextEditor::backgroundColourId, lightGrey);
  setColour(ListBox::backgroundColourId, Colours::white);
  setColour(ListBox::textColourId, Colours::black);
  setColour(SwitchComponent::colorIdBackground, lightGrey);
  setColour(SwitchComponent::colorIdHandle, chipPink);

  _232MK =
    Typeface::createSystemTypefaceFor(BinaryData::_232MKM_TTF, BinaryData::_232MKM_TTFSize);
  roboto =
    Typeface::createSystemTypefaceFor(BinaryData::RobotoRegular_ttf, BinaryData::RobotoRegular_ttfSize);
}

PokeLookAndFeel::~PokeLookAndFeel(){};

Typeface::Ptr PokeLookAndFeel::getTypefaceForFont(const Font &font) {
  return roboto;
}

void PokeLookAndFeel::drawLinearSliderThumb(Graphics &g, int x, int y, int width, int height,
                                            float sliderPos, float minSliderPos, float maxSliderPos,
                                            const Slider::SliderStyle style, Slider &slider) {
  const float radius = getSliderThumbRadius(slider);

  g.setColour(slider.findColour(Slider::thumbColourId));

  float kx, ky;

  if (style == Slider::LinearVertical) {
    kx = x + width * 0.5f;
    ky = sliderPos;
  } else {
    kx = sliderPos;
    ky = y + height * 0.5f;
  }

  Path circle;
  circle.addCentredArc(kx, ky, radius, radius, 0.0f, 0.0f, M_PI * 2.0f);
  circle.closeSubPath();

  g.fillPath(circle);
}

void PokeLookAndFeel::drawLinearSliderBackground(Graphics &g, int x, int y, int width, int height,
                                                 float sliderPos, float minSliderPos,
                                                 float maxSliderPos,
                                                 const Slider::SliderStyle style, Slider &slider) {
  const float radius = std::floor(getSliderThumbRadius(slider) * 0.333f);

  g.setColour(slider.findColour(Slider::backgroundColourId));

  Path indent;

  if (slider.isHorizontal()) {
    const float iy = y + height * 0.5f - radius;
    indent.addRoundedRectangle(x - radius, iy, width + radius * 2.0f, radius * 2.0f, radius);
  } else {
    const float ix = x + width * 0.5f - radius;
    indent.addRoundedRectangle(ix, y - radius, radius * 2.0f, height + radius * 2.0f, radius);
  }

  g.fillPath(indent);
}

void PokeLookAndFeel::drawLinearSlider(Graphics &g, int x, int y, int width, int height,
                                       float sliderPos, float minSliderPos, float maxSliderPos,
                                       const Slider::SliderStyle style, Slider &slider) {
  drawLinearSliderBackground(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style,
                             slider);
  drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style,
                        slider);
}

int PokeLookAndFeel::getSliderThumbRadius(Slider &slider) {
  return jmin(14, slider.getHeight() / 2, slider.getWidth() / 2);
}
