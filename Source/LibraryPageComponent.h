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
    virtual void handleInstallTimedOut(AppIconButton* btn) {};
  };
  
  int spawn(char** arg);
  virtual void run();
  bool isRunning();
  bool hasPending();
  
  Array<AppIconButton *> appQueue;
  
  void addListener(Listener* listener);

private:
  void emitInstallStarted();
  void emitInstallFinished();
  void emitInstallTimedOut();
  
  void clearCurrentInstall();
  
  Array<Listener *> listeners;
  
  bool installing = false;
  int installPid = 0;
  int runningCheckPid = 0;
  int waitTimeout = 0;
  
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
  void handleInstallTimedOut(AppIconButton* btn) override;
  
  void buttonClicked(Button *btn) override;
private:
  Colour bgColor;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryPageComponent)
};