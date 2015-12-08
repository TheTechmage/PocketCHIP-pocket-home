#include "Utils.h"

ImageButton *createIconButton(const String &name, const File &imageFile) {

  ImageButton *button = new ImageButton(name);
  Image image = Image(Image::RGB, 128, 128, true);

  if (imageFile.getFileExtension() == ".svg") {
    ScopedPointer<XmlElement> svgElement(XmlDocument::parse(imageFile));
    ScopedPointer<DrawableComposite> svgDrawable =
        dynamic_cast<DrawableComposite *>(Drawable::createFromSVG(*svgElement));
    svgDrawable->resetBoundingBoxToContentArea();

    Graphics g(image);
    svgDrawable->drawWithin(g, Rectangle<float>(0, 0, image.getWidth(), image.getHeight()),
                            RectanglePlacement::fillDestination, 1.0f);
  } else {
    image = ImageFileFormat::loadFrom(imageFile);
  }
  button->setImages(true, true, true, image, 1.0f, Colours::transparentWhite, // normal
                    image, 1.0f, Colours::transparentWhite,                   // over
                    image, 0.7f, Colours::transparentBlack,                   // down
                    0.5f);
  return button;
}
