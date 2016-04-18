#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppsPageComponent.h"

class DownloadsMonitor : public juce::Thread {
public:
  DownloadsMonitor();
  ~DownloadsMonitor();

  class Listener {
  public:
    Listener() {};
    virtual ~Listener() {};
    virtual void handleInstallStarted(AppIconButton* btn) {};
    virtual void handleInstallFinished(AppIconButton* btn) {};
  };
  
  virtual void run();
  bool hasPending();
  
  Array<AppIconButton *> appQueue;
  ScopedPointer<ChildProcess> installProc;
  
  void addListener(Listener* listener);

private:
  void emitInstallStarted();
  void emitInstallFinished();
  
  Array<Listener *> listeners;
  
  bool installing = false;
  
  String installAppName;
  AppIconButton * installAppBtn;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DownloadsMonitor)
};

class LibraryPageComponent :
  public AppListComponent,
  public DownloadsMonitor::Listener
{
public:
  LibraryPageComponent();
  ~LibraryPageComponent();
  
  ScopedPointer<ImageButton> backButton;
  
  DownloadsMonitor downloadsMonitor;
  
  void paint(Graphics &g) override;
  void resized() override;
  
  void handleInstallStarted(AppIconButton* btn) override;
  void handleInstallFinished(AppIconButton* btn) override;
  
  void buttonClicked(Button *btn) override;
private:
  Colour bgColor;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryPageComponent)
};