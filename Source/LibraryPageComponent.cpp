#include "LibraryPageComponent.h"

#include "PokeLookAndFeel.h"
#include "Main.h"
#include "Utils.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

DownloadsMonitor::DownloadsMonitor()
: Thread( "DownloadsMonitor" ) {
}

DownloadsMonitor::~DownloadsMonitor() {
}

void DownloadsMonitor::addListener(Listener* listener) {
  listeners.add(listener);
}

void DownloadsMonitor::emitInstallStarted(){
  const MessageManagerLock mmLock;
  DBG("DownloadsMonitor::emitInstallStarted `" << installAppName << "`");

  for (int i = 0; i < listeners.size(); i++) {
    if (listeners[i])
      listeners[i]->handleInstallStarted(installAppBtn);
  }
}

void DownloadsMonitor::emitInstallFinished(){
  const MessageManagerLock mmLock;
  DBG("DownloadsMonitor::emitInstallFinished `" << installAppName << "`");

  for (int i = 0; i < listeners.size(); i++) {
    if (listeners[i])
      listeners[i]->handleInstallFinished(installAppBtn);
  }
}

void DownloadsMonitor::emitInstallTimedOut(){
  const MessageManagerLock mmLock;
  DBG("DownloadsMonitor::emitInstallTimedOut `" << installAppName << "`");
  
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
  installAppBtn = nullptr;
  installPid = -1;
}

bool DownloadsMonitor::isRunning() {
// method 1
//  const char *pidStr = String(pid).toRawUTF8();
//  const char *checkPid[] = { "ps", "-p", pidStr, NULL };
//  runningCheckPid = spawn((char**)checkPid);
//  return (getpgid(pid) >= 0) ? true : false;
  
// method 2
//  int error = kill(pid,0);
//  if (error == 0) {
//    return true;
//  }
//  // doesn't exist
//  else if (error == ESRCH) {
//    return false;
//  }
//  else {
//    return false;
//  }
  if (installPid == -1) return false;
  
  int status;
  int wpid = waitpid(installPid, &status, WNOHANG);
  // still running
  if (wpid == 0) {
    DBG("DownloadsMonitor::isRunning - still running " << installPid);
    return true;
  }
  // status has changed
  else if (wpid == installPid) {
    DBG("DownloadsMonitor::isRunning - exited " << installPid);
    // clean exit
    if (status == 0) {
      return false;
    }
    // terminated normally via exit
    if (WIFEXITED(status)) {
      return false;
    }
    // WIP: what are other conditions besides clean exit?
    else {
      return false;
    }
  }
  // inconclusive
  else {
    // some other process has changed
    if (wpid > 0) {
      DBG("DownloadsMonitor::isRunning - found status for other process. pid: " << installPid << " otherpid: " << wpid);
      return true;
    }
    // an error ocurred
    else {
      DBG("DownloadsMonitor::isRunning - error. pid: " << installPid << " status: " << status);
      if (errno == ECHILD) {
        DBG("DownloadsMonitor::isRunning - no child process to wait for " << installPid);
      }
      return true;
    }
  }
}

int DownloadsMonitor::spawn(char** arg) {
  int pid = fork();
  
  // parent process
  if (pid > 0) {
    return pid;
  }
  // child process
  else if (pid == 0) {
    // create session and make process group leader
    setsid();
    // hide all stderr
//    freopen("/dev/null", "a", stderr);
    execvp(arg[0], arg);
    // if we're here execvp failed
    char* errstr;
    asprintf(&errstr, "pocket-home: failed to spawn - %s", arg[0]);
    perror(errstr);
    exit(1);
  }
  // fork failed
  else {
    printf("pocket-home: fork failed.");
    exit(1);
  }
}

void DownloadsMonitor::run( ) {
  constexpr auto waitTime = 2000;
  constexpr auto maxWait = 60 * 1000;
  
  while( !threadShouldExit() && hasPending() ) {
    DBG("DownloadsMonitor::run - checking download queue");

    // check installing status
    if (installing) {
      if (isRunning()) {
        DBG("DownloadsMonitor::run - running install of `" << installAppName << "`");
        waitTimeout += waitTime;
        if (waitTimeout > maxWait) {
          waitTimeout = 0;
          emitInstallTimedOut();
          clearCurrentInstall();
        }
      }
      else {
        emitInstallFinished();
        clearCurrentInstall();
      }
    }
    
    // if queued and not installing, start next install
    if (appQueue.size() && !installing) {
      installAppBtn = appQueue.getFirst();
      installAppName = installAppBtn->shell;
      installing = true;
//      const char *installCmd[] = {"sh", "-c", ("sudo apt-get --yes install " + installAppName).toRawUTF8(), NULL};
      const char *installCmd[] = {"sudo", "apt-get", "--yes", "install", installAppName.toRawUTF8(), NULL};
      installPid = spawn((char**)installCmd);
      
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
  btn->setColour(DrawableButton::textColourId, Colours::yellow);
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