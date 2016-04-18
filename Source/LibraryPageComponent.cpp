#include "LibraryPageComponent.h"

#include "PokeLookAndFeel.h"
#include "Main.h"
#include "Utils.h"

DownloadsMonitor::DownloadsMonitor( )
: Thread( "DownloadsMonitor" ) {
}

DownloadsMonitor::~DownloadsMonitor( ) {
}

bool DownloadsMonitor::hasPending( ) {
  return (installing || !appQueue.empty());
}

void DownloadsMonitor::run( ) {
  while( !threadShouldExit() && hasPending() ) {
    DBG("DownloadsMonitor::run - checking download queue");

    // check installing status
    if (installing) {
      if (installProc->isRunning()) {
        DBG("DownloadsMonitor::run - running install ");
      }
      else {
        DBG("DownloadsMonitor::run - finished install ");
        installing = false;
        installProc = nullptr;
      }
    }
    
    // if queued and not installing, start next install
    if (appQueue.size() && !installing) {
      DBG("DownloadsMonitor::run - beginning install");
      auto first = appQueue.getFirst();
      StringArray installCmd{"sudo", "apt-get", "--yes", "install", first.toRawUTF8()};
      installProc = new ChildProcess();
      
      installing = true;
      installProc->start(installCmd);
      
      appQueue.remove(0);
    }
    
    wait(2000);
  }
}

LibraryPageComponent::LibraryPageComponent() :
  AppListComponent(),
  backButton(createImageButton("Back",
                               createImageFromFile(assetFile("nextIcon.png")))),
  downloadsMonitor()
{
  bgColor = Colour(PokeLookAndFeel::chipPurple);
  
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

void LibraryPageComponent::buttonClicked(Button *button) {
  if (button == backButton) {
    getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontalLeft);
  }
  else if (button == prevPageBtn) {
    train->showPrevPage();
    checkShowPageNav();
  }
  else if (button == nextPageBtn) {
    train->showNextPage();
    checkShowPageNav();
  }
  else {
    auto& appName = ((AppIconButton*)button)->shell;
    downloadsMonitor.appQueue.add(appName);
    downloadsMonitor.startThread();
  }
}