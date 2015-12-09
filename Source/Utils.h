#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

File absoluteFileFromPath(const String &path);

ImageButton *createIconButton(const String &name, const File &imageFile);
void drawDrawableToImage(Image *image, const Drawable &drawable);
