#include "LibraryPageComponent.h"

#include "PokeLookAndFeel.h"
#include "Main.h"
#include "Utils.h"

DownloadsMonitor::DownloadsMonitor( )
: Thread( "DownloadsMonitor" ) {
}

DownloadsMonitor::~DownloadsMonitor( ) {
}

void DownloadsMonitor::addListener(Listener* listener) {
  listeners.add(listener);
}

void DownloadsMonitor::emitInstallStarted(){
  const MessageManagerLock mmLock;

  for (int i = 0; i < listeners.size(); i++) {
    if (listeners[i])
      listeners[i]->handleInstallStarted(installAppBtn);
  }
}

void DownloadsMonitor::emitInstallFinished(){
  const MessageManagerLock mmLock;

  for (int i = 0; i < listeners.size(); i++) {
    if (listeners[i])
      listeners[i]->handleInstallFinished(installAppBtn);
  }
}

void DownloadsMonitor::emitInstallTimedOut(){
  const MessageManagerLock mmLock;
  
  for (int i = 0; i < listeners.size(); i++) {
    if (listeners[i])
      listeners[i]->handleInstallTimedOut(installAppBtn);
  }
}

bool DownloadsMonitor::hasPending( ) {
  return (installing || !appQueue.empty());
}

void DownloadsMonitor::clearCurrentInstall() {
  installing = false;
  installAppName = String::empty;
  installProc = nullptr;
}

void DownloadsMonitor::run( ) {
  constexpr auto waitTime = 2000;
  
  while( !threadShouldExit() && hasPending() ) {
    DBG("DownloadsMonitor::run - checking download queue");

    // check installing status
    if (installing) {
      if (installProc->isRunning()) {
        DBG("DownloadsMonitor::run - running install of `" << installAppName << "`");
        waitTimeout += waitTime;
        if (waitTimeout > (60 * 1000)) {
          waitTimeout = 0;
          emitInstallTimedOut();
          clearCurrentInstall();
        }
      }
      else {
        DBG("DownloadsMonitor::run - finished install `" << installAppName << "`");
        emitInstallFinished();
        clearCurrentInstall();
      }
    }
    
    // if queued and not installing, start next install
    if (appQueue.size() && !installing) {
      installAppBtn = appQueue.getFirst();
      installAppName = installAppBtn->shell;
      DBG("DownloadsMonitor::run - beginning install of `" << installAppName << "`");
      StringArray installCmd{"sudo", "apt-get", "--yes", "install", installAppName.toRawUTF8()};
      installProc = new ChildProcess();
      
      installing = true;
      installProc->start(installCmd);
      
      emitInstallStarted();
      
      appQueue.remove(0);
    }
    
    wait(waitTime);
  }
}

LibraryPageComponent::LibraryPageComponent() :
  AppListComponent(),
  backButton(createImageButton("Back",
                               createImageFromFile(assetFile("nextIcon.png")))),
  downloadsMonitor()
{
  bgColor = Colour(PokeLookAndFeel::chipPurple);
  
  downloadsMonitor.addListener(this);
  
  backButton->addListener(this);
  backButton->setTriggeredOnMouseDown(true);
  backButton->setAlwaysOnTop(true);
  addAndMakeVisible(backButton);
}

LibraryPageComponent::~LibraryPageComponent() {
  // TODO: determine good timeout on download thread destruction
  downloadsMonitor.stopThread(5000);
}

void LibraryPageComponent::paint(Graphics &g) {
  g.fillAll(bgColor);
}

void LibraryPageComponent::resized() {
  AppListComponent::resized();
  
  const auto& b = getLocalBounds();
  auto trainWidth = b.getWidth() - 2*btnHeight;
  auto trainHeight = b.getHeight() - (2.1*btnHeight);
  train->setSize(trainWidth, trainHeight);
  train->setBoundsToFit(b.getX(), b.getY(), b.getWidth(), b.getHeight(), Justification::centred, true);
  
  backButton->setBounds(b.getWidth()-60, b.getY(), 60, b.getHeight());
}

void LibraryPageComponent::handleInstallTimedOut(AppIconButton* btn) {
  btn->removeColour(DrawableButton::backgroundColourId);
  btn->setColour(DrawableButton::textColourId, Colours::indianred);
}

void LibraryPageComponent::handleInstallStarted(AppIconButton* btn) {
  btn->setColour(DrawableButton::backgroundColourId, PokeLookAndFeel::chipLightPink);
  btn->setColour(DrawableButton::textColourId, Colours::white);
  btn->setAlpha(0.8);
}

void LibraryPageComponent::handleInstallFinished(AppIconButton* btn) {
  btn->removeColour(DrawableButton::backgroundColourId);
  btn->setColour(DrawableButton::textColourId, Colours::white);
  btn->setAlpha(1.0);
}

void LibraryPageComponent::buttonClicked(Button *btn) {
  if (btn == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontalLeft);
  }
  else if (btn == prevPageBtn) {
    train->showPrevPage();
    checkShowPageNav();
  }
  else if (btn == nextPageBtn) {
    train->showNextPage();
    checkShowPageNav();
  }
  else {
    const auto& appBtn = (AppIconButton*)btn;
    if ( !(downloadsMonitor.appQueue.indexOf(appBtn) >= 0) ) {
      downloadsMonitor.appQueue.add(appBtn);
      appBtn->setColour(DrawableButton::textColourId, PokeLookAndFeel::chipLightPink);
      appBtn->setAlpha(0.5);
      downloadsMonitor.startThread();
    }
  }
}