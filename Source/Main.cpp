#include "Main.h"
#include "MainComponent.h"
#include "WifiStatus.h"
#include "Utils.h"

// FIXME: this is a hack to fix touch screen presses causing buzzing
// when no application holds alsa open
#if JUCE_LINUX
#include <alsa/asoundlib.h>

#define DEFAULT_BUFFER_SIZE	4096	/*in samples*/
snd_pcm_t *g_alsa_playback_handle = 0;
#endif

void BluetoothStatus::populateFromJson(const var &json) {
  devices.clear();

  for (const auto &btDevice : *json.getArray()) {
    auto device = new BluetoothDevice();
    device->name = btDevice["name"].toString();
    device->macAddress = btDevice["mac"].toString();
    device->connected = btDevice["connected"];
    device->paired = btDevice["paired"];
    devices.add(device);
  }
}

PageStackComponent &getMainStack() {
  return PokeLaunchApplication::get()->getMainStack();
}

WifiStatus &getWifiStatus() {
  return *PokeLaunchApplication::get()->wifiStatus;
}

BluetoothStatus &getBluetoothStatus() {
  return PokeLaunchApplication::get()->bluetoothStatus;
}

ImageComponent &getMainButtonPopup() {
  return *getMainContentComponent().buttonPopup;
}

MainContentComponent &getMainContentComponent() {
  return *static_cast<MainContentComponent*>( PokeLaunchApplication::get()->getMainWindow()->getContentComponent() );
}

PokeLaunchApplication::PokeLaunchApplication() {}

PokeLaunchApplication *PokeLaunchApplication::get() {
  return dynamic_cast<PokeLaunchApplication *>(JUCEApplication::getInstance());
}

const String PokeLaunchApplication::getApplicationName() {
  return ProjectInfo::projectName;
}

const String PokeLaunchApplication::getApplicationVersion() {
  return ProjectInfo::versionString;
}

bool PokeLaunchApplication::moreThanOneInstanceAllowed() {
  return false;
}

bool PokeLaunchApplication::sound() {
#if JUCE_LINUX
  int err;
  int freq = 44100, channels = 2;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_sw_params_t *sw_params;

  snd_pcm_hw_params_malloc( &hw_params );
  snd_pcm_sw_params_malloc( &sw_params );
  err = snd_pcm_open( &g_alsa_playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0 );
  if( err < 0 )
  {
  	 DBG( "ALSA ERROR: Can't open audio device: " << snd_strerror( err ) );
	   return false;
  }
  DBG("Opened Audio Device");
  err = snd_pcm_hw_params_any( g_alsa_playback_handle, hw_params );
  if( err < 0 )
  {
	    DBG( "ALSA ERROR: Can't initialize hardware parameter structure: " << snd_strerror( err ) );
	    return false;
  }
  err = snd_pcm_hw_params_set_access( g_alsa_playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED );
  if( err < 0 )
  {
      DBG( "ALSA ERROR: Can't set access type: " << snd_strerror( err ) );
      return false;
  }
  //const UTF8_CHAR *sample_format = "";
  err = snd_pcm_hw_params_set_format( g_alsa_playback_handle, hw_params, SND_PCM_FORMAT_S16_LE );
  if( err < 0 )
  {
	    DBG( "ALSA ERROR: Can't set sample format :" << snd_strerror( err ) );
	    return false;
  }
  err = snd_pcm_hw_params_set_rate_near( g_alsa_playback_handle, hw_params, (unsigned int*)&freq, 0 );
  if( err < 0 )
  {
      DBG( "ALSA ERROR: Can't set sample rate: " << snd_strerror( err ) );
	    return false;
  }
  DBG( "ALSA Sample rate: "<< freq );
  err = snd_pcm_hw_params_set_channels( g_alsa_playback_handle, hw_params, channels );
  if( err < 0 )
  {
	    DBG( "ALSA ERROR: Can't set channel count: " << snd_strerror( err ) );
	    return false;
  }
  snd_pcm_uframes_t frames;
	frames = DEFAULT_BUFFER_SIZE;
  err = snd_pcm_hw_params_set_buffer_size_near( g_alsa_playback_handle, hw_params, &frames );
  if( err < 0 )
  {
	    DBG( "ALSA ERROR: Can't set buffer size: " << snd_strerror( err ) );
	    return false;
  }
  snd_pcm_hw_params_get_buffer_size( hw_params, &frames );
  DBG( "ALSA Buffer size: 4096 samples" );
  err = snd_pcm_hw_params( g_alsa_playback_handle, hw_params );
  if( err < 0 )
  {
	   DBG( "ALSA ERROR: Can't set parameters: " << snd_strerror( err ) );
	    return false;
  }
  snd_pcm_hw_params_free( hw_params );
  snd_pcm_sw_params_free( sw_params );

  err = snd_pcm_prepare( g_alsa_playback_handle );
  if( err < 0 )
  {
	   DBG( "ALSA ERROR: Can't prepare audio interface for use: " << snd_strerror( err ) );
	  return false;
  }

  /* Stop PCM device and drop pending frames */
  snd_pcm_drain(g_alsa_playback_handle);
#endif

  return true;
}

void PokeLaunchApplication::initialise(const String &commandLine) {
  StringArray args;
  args.addTokens(commandLine, true);

  if (args.contains("--help")) {
    std::cerr << "arguments:" << std::endl;
    std::cerr << "  --help:	Print usage help" << std::endl;
    std::cerr << "  --fakewifi:	Use fake WifiStatus" << std::endl;
    quit();
  }

  auto configFile = assetFile("config.json");
  if (!configFile.exists()) {
    std::cerr << "Missing config file: " << configFile.getFullPathName() << std::endl;
    quit();
  }

  auto configJson = JSON::parse(configFile);
  if (!configJson) {
    std::cerr << "Could not parse config file: " << configFile.getFullPathName() << std::endl;
    quit();
  }

  // open sound handle

  if(!sound())
    DBG("Sound failed to initialize");

  // Populate with dummy data
  {
    if (args.contains("--fakewifi"))
      wifiStatus = &wifiStatusJson;
    else
      wifiStatus = &wifiStatusNM;

    wifiStatus->initializeStatus();

    auto deviceListFile = assetFile("bluetooth.json");
    bluetoothStatus.populateFromJson(JSON::parse(deviceListFile));
  }

  mainWindow = new MainWindow(getApplicationName(), configJson);
}

void PokeLaunchApplication::shutdown() {
  // Add your application's shutdown code here..

  mainWindow = nullptr; // (deletes our window)
}

void PokeLaunchApplication::systemRequestedQuit() {
  // This is called when the app is being asked to quit: you can ignore this
  // request and let the app carry on running, or call quit() to allow the app to close.
  quit();
}

void PokeLaunchApplication::anotherInstanceStarted(const String &commandLine) {
  // When another instance of the app is launched while this one is running,
  // this method is invoked, and the commandLine parameter tells you what
  // the other instance's command-line arguments were.
}

PageStackComponent &PokeLaunchApplication::getMainStack() {
  return *dynamic_cast<MainContentComponent *>(mainWindow->getContentComponent())->pageStack;
}

PokeLaunchApplication::MainWindow* PokeLaunchApplication::getMainWindow() {
  return mainWindow.get();
}

PokeLaunchApplication::MainWindow::MainWindow(String name, const var &configJson)
: DocumentWindow(name, Colours::darkgrey, DocumentWindow::allButtons) {
  setUsingNativeTitleBar(true);
  setResizable(true, false);
  setContentOwned(new MainContentComponent(configJson), true);
  centreWithSize(getWidth(), getHeight());
  setVisible(true);
#if JUCE_LINUX
//  setTitleBarHeight(0);
//  setFullScreen(true);
#endif
}

void PokeLaunchApplication::MainWindow::activeWindowStatusChanged() {
  if (!isActiveWindow()) {
    auto contentComponent = getContentComponent();
    if (contentComponent) {
      ((MainContentComponent*)contentComponent)->handleMainWindowInactive();
    }
  }
}

void PokeLaunchApplication::MainWindow::closeButtonPressed() {
  // This is called when the user tries to close this window. Here, we'll just
  // ask the app to quit when this happens, but you can change this to do
  // whatever you need.
  JUCEApplication::getInstance()->systemRequestedQuit();
}

START_JUCE_APPLICATION(PokeLaunchApplication)
