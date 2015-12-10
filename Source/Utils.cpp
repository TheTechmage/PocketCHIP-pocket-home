#include "Utils.h"

File absoluteFileFromPath(const String &path) {
  return File::isAbsolutePath(path) ? File(path)
                                    : File::getCurrentWorkingDirectory().getChildFile(path);
}

Image createImageFromFile(const File &imageFile) {
  auto image = Image(Image::RGB, 128, 128, true);
  if (imageFile.getFileExtension() == ".svg") {
    ScopedPointer<XmlElement> svgElement = XmlDocument::parse(imageFile);
    ScopedPointer<Drawable> svgDrawable = Drawable::createFromSVG(*svgElement);
    Graphics g(image);
    svgDrawable->drawWithin(g, Rectangle<float>(0, 0, image.getWidth(), image.getHeight()),
                            RectanglePlacement::fillDestination, 1.0f);
  } else {
    image = ImageFileFormat::loadFrom(imageFile);
  }
  return image;
}

ImageButton *createImageButton(const String &name, const File &imageFile) {
  auto button = new ImageButton(name);
  auto image = createImageFromFile(imageFile);
  button->setImages(true, true, true,                       //
                    image, 1.0f, Colours::transparentWhite, // normal
                    image, 1.0f, Colours::transparentWhite, // over
                    image, 0.7f, Colours::transparentBlack, // down
                    0.5f);
  return button;
}

void fitRectInRect(Rectangle<int> &rect, int x, int y, int width, int height,
                   Justification justification, const bool onlyReduceInSize) {
  // it's no good calling this method unless both the component and
  // target rectangle have a finite size.
  jassert(rect.getWidth() > 0 && rect.getHeight() > 0 && width > 0 && height > 0);

  if (rect.getWidth() > 0 && rect.getHeight() > 0 && width > 0 && height > 0) {
    int newW, newH;

    if (onlyReduceInSize && rect.getWidth() <= width && rect.getHeight() <= height) {
      newW = rect.getWidth();
      newH = rect.getHeight();
    } else {
      const double imageRatio = rect.getHeight() / (double)rect.getWidth();
      const double targetRatio = height / (double)width;

      if (imageRatio <= targetRatio) {
        newW = width;
        newH = jmin(height, roundToInt(newW * imageRatio));
      } else {
        newH = height;
        newW = jmin(width, roundToInt(newH / imageRatio));
      }
    }

    if (newW > 0 && newH > 0)
      rect = justification.appliedToRectangle(Rectangle<int>(newW, newH),
                                              Rectangle<int>(x, y, width, height));
  }
}

void fitRectInRect(Rectangle<int> &rect, const Rectangle<int> &container,
                   Justification justification, const bool onlyReduceInSize) {
  fitRectInRect(rect, container.getX(), container.getY(), container.getWidth(),
                container.getHeight(), justification, onlyReduceInSize);
}

float smoothstep(float edge0, float edge1, float x) {
  x = std::min(std::max((x - edge0) / (edge1 - edge0), 0.0f), 1.0f);
  return x * x * (3.0f - 2.0f * x);
}

float mix(float a, float b, float t) {
  return t * (b - a) + a;
}

void animateTranslation(Component *component, int x, int y, float alpha, int durationMillis) {
  const auto &bounds = component->getBounds();
  auto destBounds = bounds.translated(x - bounds.getX(), y - bounds.getY());
  Desktop::getInstance().getAnimator().animateComponent(component, destBounds, alpha,
                                                        durationMillis, false, 0, 0);
}
