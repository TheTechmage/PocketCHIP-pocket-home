#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

File absoluteFileFromPath(const String &path);

Image createImageFromFile(const File &imageFile);
ImageButton *createImageButton(const String &name, const File &imageFile);
ImageButton *createImageButtonFromDrawable(const String &name, const Drawable &drawable);

void fitRectInRect(Rectangle<int> &rect, int x, int y, int width, int height,
                   Justification justification, const bool onlyReduceInSize);
void fitRectInRect(Rectangle<int> &rect, const Rectangle<int> &container,
                   Justification justification, const bool onlyReduceInSize);

float smoothstep(float edge0, float edge1, float x);
float mix(float a, float b, float t);
float mapLinear(float x, float imin, float imax, float omin, float omax);

void animateTranslation(Component *component, int x, int y, float alpha, int durationMillis);
