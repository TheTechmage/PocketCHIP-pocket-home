#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class PokeLookAndFeel : public LookAndFeel_V3 {

public:
  PokeLookAndFeel();
  ~PokeLookAndFeel();

  Typeface::Ptr getTypefaceForFont(const Font& font) override;

  void drawLinearSliderThumb(Graphics &g, int x, int y, int width, int height, float sliderPos,
                             float minSliderPos, float maxSliderPos,
                             const Slider::SliderStyle style, Slider &slider) override;
  void drawLinearSliderBackground(Graphics &g, int x, int y, int width, int height, float sliderPos,
                                  float minSliderPos, float maxSliderPos,
                                  const Slider::SliderStyle style, Slider &slider) override;
  void drawLinearSlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                        float minSliderPos, float maxSliderPos, const Slider::SliderStyle style,
                        Slider &slider) override;

  int getSliderThumbRadius(Slider &slider) override;

  void drawButtonText (Graphics& g, TextButton& button, bool /*isMouseOverButton*/,
                       bool /*isButtonDown*/)override;

  void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                             bool isMouseOverButton, bool isButtonDown) override;

private:
  Typeface::Ptr _232MK, roboto;

  Colour lightGrey;
  Colour medGrey;
  Colour chipPink;

};
