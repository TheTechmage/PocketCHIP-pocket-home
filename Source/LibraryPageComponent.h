#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"

class DownloadsMonitor: public juce::Thread {
public:
  DownloadsMonitor();
  ~DownloadsMonitor();
  
  virtual void run();
  bool hasPending();
  
  Array<String> appQueue;
  ScopedPointer<ChildProcess> installProc;

private:
  bool installing = false;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DownloadsMonitor)
};

class LibraryPageComponent : public AppListComponent {
public:
  LibraryPageComponent();
  ~LibraryPageComponent();
  
  ScopedPointer<ImageButton> backButton;
  
  DownloadsMonitor downloadsMonitor;
  
  void paint(Graphics &g) override;
  void resized() override;
  
  void buttonClicked(Button *button) override;
private:
  Colour bgColor;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryPageComponent)
};