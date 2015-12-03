#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

struct PokeLookAndFeel : public LookAndFeel_V3 {
  PokeLookAndFeel();

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
};
