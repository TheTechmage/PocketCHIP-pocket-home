#include "PokeLookAndFeel.h"
#include "SwitchComponent.h"
#include "Utils.h"
#include "LauncherBarComponent.h"
#include "Main.h"

Colour PokeLookAndFeel::lightGrey = Colour(0xffe1e1e1);
Colour PokeLookAndFeel::medGrey = Colour(0xffc0c0c0);
Colour PokeLookAndFeel::chipPink = Colour(0xffbc3662);
Colour PokeLookAndFeel::chipLightPink = Colour(0xfff799aa);
Colour PokeLookAndFeel::chipPurple = Colour(0xffd23c6d);

PokeLookAndFeel::PokeLookAndFeel() {
  setColour(DrawableButton::textColourId, Colours::white);
  setColour(TextButton::buttonColourId, Colours::white);
  setColour(TextButton::buttonOnColourId, Colours::white);
  setColour(TextButton::textColourOnId, Colours::white);
  setColour(TextButton::textColourOffId, Colours::white);
  setColour(Label::textColourId, Colours::white);
  setColour(Slider::backgroundColourId, chipLightPink);
  setColour(Slider::thumbColourId, Colours::white);
  setColour(TextEditor::backgroundColourId, chipLightPink);
  setColour(TextEditor::textColourId, Colours::white);
  setColour(ListBox::backgroundColourId, Colours::white);
  setColour(ListBox::textColourId, Colours::white);
  setColour(SwitchComponent::colorIdBackground, chipLightPink);
  setColour(SwitchComponent::colorIdHandle, Colours::white);
  setColour(SwitchComponent::colorIdHandleOff, chipPink);

  seguibl = Typeface::createSystemTypefaceFor(BinaryData::LatoRegular_ttf,
                                             BinaryData::LatoRegular_ttfSize);
}

PokeLookAndFeel::~PokeLookAndFeel(){};

float PokeLookAndFeel::getButtonHeight() {
  float height = 1.0f;
  
  if (PokeLaunchApplication::get()->getMainWindow())
    height = jmin(getMainContentComponent().getWidth(), getMainContentComponent().getHeight()) / 5.0f;
  else
    height = 60.f;
  
  return height;
}

float PokeLookAndFeel::getDrawableButtonTextHeightForBounds(const Rectangle<int> &bounds) {
  return jmin(23.0f, bounds.getHeight() * 0.99f);
}

float PokeLookAndFeel::getDrawableButtonImageHeightForBounds(const Rectangle<int> &bounds) {
  static const float padding = 5.0f;
  return bounds.getHeight() - (getDrawableButtonTextHeightForBounds(bounds) + padding);
}

Typeface::Ptr PokeLookAndFeel::getTypefaceForFont(const Font &font) {
  return seguibl;
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
    indent.addRoundedRectangle(x - radius, iy, width + radius * 2.0f, radius * 2.0f, 1);
  } else {
    const float ix = x + width * 0.5f - radius;
    indent.addRoundedRectangle(ix, y - radius, radius * 2.0f, height + radius * 2.0f, 1);
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
  return jmin(slider.getHeight() * 0.33f, slider.getWidth() * 0.33f);
}

void PokeLookAndFeel::drawButtonText(Graphics &g, TextButton &button, bool isMouseOverButton,
                                     bool isButtonDown) {
  Font font(getTextButtonFont(button, button.getHeight()));
  font.setExtraKerningFactor(0.06f);
  font.setHeight(24);
  g.setFont(font);
  g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
                                                        : TextButton::textColourOffId)
                  .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

  const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
  const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

  const int fontHeight = roundToInt(font.getHeight() * 0.6f);
  const int leftIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
  const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));

  g.drawFittedText(button.getButtonText(), leftIndent, yIndent,
                   button.getWidth() - leftIndent - rightIndent, button.getHeight() - yIndent * 2,
                   Justification::centred, 2);
}

void PokeLookAndFeel::drawButtonBackground(Graphics &g, Button &button,
                                           const Colour &backgroundColour, bool isMouseOverButton,
                                           bool isButtonDown) {
  const int width = button.getWidth();
  const int height = button.getHeight();

  auto path = Path();
  path.addRoundedRectangle(0, 0, width, height, 1);
  g.setColour(chipPink);
  g.fillPath(path);
}

// FIXME: to support touch areas of proportional size from the base image,
// we need to explicitly size the images within image buttons when necessary,
// rather than relying on the automatic image resizing of ImageButton (it's either 100% resize or disabled).
// Otherwise images are forced to resize to the full size of the ImageButton.
void PokeLookAndFeel::drawImageButton(Graphics& g, Image* image,
                                      int imageX, int imageY, int imageW, int imageH,
                                      const Colour& overlayColour,
                                      float imageOpacity,
                                      ImageButton& button)
{
  if (! button.isEnabled())
    imageOpacity *= 0.3f;
  
  AffineTransform t;
  // FIXME: yes, runtime reflection really is the best way to override basic behavior of ImageButton
  // (and many other JUCE classes whose public overrideable functions depend on private members)
  auto launcherBtn = dynamic_cast<LauncherBarButton*>(&button);
  if (launcherBtn) {
    float a = (float)imageH / imageW;
    int iH = launcherBtn->imageHeight;
    int iW = iH / a;
    int iX = imageX + (imageW - iW) / 2.f;
    int iY = imageY + (imageH - iH) / 2.f;
    t = RectanglePlacement(RectanglePlacement::stretchToFit)
      .getTransformToFit(image->getBounds().toFloat(),
                         Rectangle<int>(iX, iY, iW, iH).toFloat());
  }
  else {
    t = RectanglePlacement(RectanglePlacement::stretchToFit)
      .getTransformToFit(image->getBounds().toFloat(),
                         Rectangle<int>(imageX, imageY, imageW, imageH).toFloat());
  }
  
  if (! overlayColour.isOpaque())
  {
    g.setOpacity (imageOpacity);
    g.drawImageTransformed (*image, t, false);
  }
  
  if (! overlayColour.isTransparent())
  {
    g.setColour (overlayColour);
    g.drawImageTransformed (*image, t, true);
  }
}

void PokeLookAndFeel::drawDrawableButton(Graphics &g, DrawableButton &button,
                                         bool isMouseOverButton, bool isButtonDown) {
  bool toggleState = button.getToggleState();

  g.fillAll(button.findColour(toggleState ? DrawableButton::backgroundOnColourId
                                          : DrawableButton::backgroundColourId));

  const int textH = (button.getStyle() == DrawableButton::ImageAboveTextLabel)
                        ? getDrawableButtonTextHeightForBounds(button.getLocalBounds())
  
                        : 0;

  Font font(18);
  font.setExtraKerningFactor(0.06f);
  if (textH > 0) {
    g.setFont(font);
    //g.setFont(textH);
    g.setColour(button.findColour(toggleState ? DrawableButton::textColourOnId
                                              : DrawableButton::textColourId)
                    .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.4f));

    g.drawFittedText(button.getButtonText(), 2, button.getHeight() - textH - 1,
                     button.getWidth() - 4, textH, Justification::centred, 1);
  }
}

MouseCursor PokeLookAndFeel::getMouseCursorFor(Component &component) {
#ifndef _DEBUG
  return MouseCursor::NoCursor;
#else
  return LookAndFeel_V3::getMouseCursorFor(component);
#endif
}
