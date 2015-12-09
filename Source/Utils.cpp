#include "Utils.h"

File absoluteFileFromPath(const String &path) {
  return File::isAbsolutePath(path) ? File(path)
                                    : File::getCurrentWorkingDirectory().getChildFile(path);
}

ImageButton *createIconButton(const String &name, const File &imageFile) {
  auto button = new ImageButton(name);
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

  button->setImages(true, true, true,                       //
                    image, 1.0f, Colours::transparentWhite, // normal
                    image, 1.0f, Colours::transparentWhite, // over
                    image, 0.7f, Colours::transparentBlack, // down
                    0.5f);

  return button;
}
