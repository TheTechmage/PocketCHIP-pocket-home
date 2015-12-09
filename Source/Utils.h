#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

File absoluteFileFromPath(const String &path);

Image createImage(const File &imageFile);
ImageButton *createImageButton(const String &name, const File &imageFile);

void fitRectInRect(Rectangle<int> &rect, int x, int y, int width, int height,
                   Justification justification, const bool onlyReduceInSize);
void fitRectInRect(Rectangle<int> &rect, const Rectangle<int> &container,
                   Justification justification, const bool onlyReduceInSize);

float smoothstep(float edge0, float edge1, float x);
float mix(float a, float b, float t);
