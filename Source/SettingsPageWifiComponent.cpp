#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsPageWifiComponent.h"
#include "Utils.h"

struct SourceItemListboxContents : public ListBoxModel {
  int getNumRows() override {
    return 15;
  }
  void paintListBoxItem(int rowNumber, Graphics &g, int width, int height,
                        bool rowIsSelected) override {
    if (rowIsSelected) {
      g.fillAll(Colours::lightgrey);
    }
    g.setColour(Colours::black);
    g.setFont(height * 0.7f);
    g.drawText("WIFI SSID #" + String(rowNumber + 1), 5, 0, width, height,
               Justification::centredLeft, true);
  }
};


SettingsPageWifiComponent::SettingsPageWifiComponent() {
  ScopedPointer<XmlElement> wifiSvg = XmlDocument::parse(BinaryData::wifiIcon_svg);
  wifiIcon = Drawable::createFromSVG(*wifiSvg);
  addAndMakeVisible(wifiIcon);

  switchComponent = new SwitchComponent();
  switchComponent->addListener(this);
  addAndMakeVisible(switchComponent);

  ssidList = new ListBox();
  ssidListModel = new SourceItemListboxContents();
  ssidList->setModel(ssidListModel);
  ssidList->setMultipleSelectionEnabled(false);

  ssidListPanel = new Component();
  addChildComponent(ssidListPanel);
  ssidListPanel->addAndMakeVisible(ssidList);
}

SettingsPageWifiComponent::~SettingsPageWifiComponent() {}

void SettingsPageWifiComponent::setWifiEnabled(Boolean enabled) {
  //  wifiPanel->setVisible(enabled);
  ssidListPanel->setVisible(enabled);
}

void SettingsPageWifiComponent::paint(Graphics &g) {}

void SettingsPageWifiComponent::resized() {
  //  wifiPanel->setVisible(wifiEnabled);

  auto bounds = getLocalBounds();

  ssidListPanel->setBounds(120, 0, bounds.getWidth() - 100, bounds.getHeight());
  ssidList->setBounds(0, 0, 200, bounds.getHeight());

  wifiIcon->setTopLeftPosition(bounds.getX(), bounds.getHeight() / 2.0f - 20);
  switchComponent->setTopLeftPosition(bounds.getX() + 80, bounds.getHeight() / 2.0);
}

void SettingsPageWifiComponent::buttonClicked(Button *button) {}

void SettingsPageWifiComponent::buttonStateChanged(Button *button) {
  if (button == switchComponent) {
    if (wifiEnabled != button->getToggleState()) {
      wifiEnabled = button->getToggleState();
      setWifiEnabled(wifiEnabled);
    }
  }
}
