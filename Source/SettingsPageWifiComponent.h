#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "SwitchComponent.h"
#include "PageStackComponent.h"
#include "Grid.h"
#include "Main.h"

class SettingsPageWifiComponent;
class WifiSpinnerTimer;
class WifiSpinner;

class WifiSpinnerTimer : public Timer {
public:
  void timerCallback() override;
  
  WifiSpinner* spinner;
};


class WifiSpinner : public ImageComponent {
public:
  WifiSpinner(const String& componentName = String::empty);
  ~WifiSpinner();
  
  void hide();
  void show();
  void nextImage();
  
private:
  Array<Image> images;
  WifiSpinnerTimer timer;
  
  int i = 0;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WifiSpinner)
};

struct WifiIcons {
  OwnedArray<Drawable> wifiStrength;
  ScopedPointer<Drawable> lockIcon;
  ScopedPointer<Drawable> arrowIcon;
};

class WifiAccessPointListItem : public Button {
public:
  WifiAccessPoint *ap;

  WifiAccessPointListItem(WifiAccessPoint *ap, WifiIcons *icons);

  void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;

private:
  WifiIcons *icons;

  int wifiSignalStrengthToIdx(int strength);
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WifiAccessPointListItem)
};

class SettingsPageWifiComponent : public Component,
  public WifiStatus::Listener,
  private Button::Listener, private TextEditor::Listener, private ComponentListener {
public:
  SettingsPageWifiComponent();
  ~SettingsPageWifiComponent();

  ScopedPointer<WifiAccessPoint> selectedAp;

  ScopedPointer<PageStackComponent> pageStack;

  ScopedPointer<ImageButton> backButton;
  ScopedPointer<ImageComponent> wifiIconComponent;
  ScopedPointer<WifiIcons> icons;
  
  ScopedPointer<WifiSpinner> spinner;

  ScopedPointer<Component> accessPointListPage;
  ScopedPointer<Grid> accessPointList;
  ScopedPointer<ImageButton> nextPageBtn;
  ScopedPointer<ImageButton> prevPageBtn;

  ScopedPointer<Component> connectionPage;
  ScopedPointer<Label> connectionLabel;
  ScopedPointer<TextEditor> passwordEditor;
  ScopedPointer<TextButton> connectionButton;
  ScopedPointer<Label> errorLabel;

  ScopedPointer<SwitchComponent> switchComponent;

  OwnedArray<WifiAccessPointListItem> accessPointItems;
    
  void updateAccessPoints();

  void paint(Graphics &g) override;
  void resized() override;
  
  void handleWifiEnabled() override;
  void handleWifiDisabled() override;
  void handleWifiConnected() override;
  void handleWifiDisconnected() override;
  void handleWifiFailedConnect() override;
  void handleWifiBusy() override;

private:
  Colour bgColor;
  Image bgImage;
  
  bool init = false;
  
  void enableWifiActions();
  void disableWifiActions();
  void beginSetConnected();
  void beginSetDisconnected();
  void updateConnectionLabelAndButton();
  void createAccessPointList();
  void checkShowListNav();

  void buttonStateChanged(Button*) override;
  void buttonClicked(Button*) override;
  void textEditorReturnKeyPressed(TextEditor &) override;
  void componentVisibilityChanged(Component &) override;
  
  OwnedArray<WifiAccessPoint> accessPoints;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPageWifiComponent)
};
