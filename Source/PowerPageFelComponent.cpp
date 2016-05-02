#include "PowerPageFelComponent.h"
#include "Main.h"
#include "Utils.h"
#include "PokeLookAndFeel.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <numeric>

#if JUCE_LINUX

#include <linux/i2c-dev.h>

int i2c_dev_open( const char* i2cdev, uint8_t address ) {
    int file;
    if ( (file = open( i2cdev, O_RDWR ) ) < 0 ) {
        printf("Failed to open the bus.\n");
        printf("Error: %s\n", strerror( errno ) );
        return 0;
    }

    if ( ioctl(file, I2C_SLAVE_FORCE, address) < 0 ) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        printf("Error: %s\n", strerror( errno ) );
        return 0;
    }
    return file;
}

uint8_t i2c_write_byte(int file, uint8_t reg, uint8_t byte) {
    uint8_t res = i2c_smbus_write_byte_data(file, reg, byte);
    
    if( res < 0 )
        printf("Error: %s\n", strerror( errno ) );
    
    return res;
}

uint8_t i2c_read_byte(int file, uint8_t reg) {
    uint8_t data = i2c_smbus_read_byte_data(file, reg);
    return data;
}

#else

// MacOSX i2c stubs
int i2c_dev_open( const char* i2cdev, uint8_t address ) { return 0; }
uint8_t i2c_write_byte(int file, uint8_t reg, uint8_t byte) { return 0; }
uint8_t i2c_read_byte(int file, uint8_t reg) { return 0; }

#endif

PowerFelCategoryButton::PowerFelCategoryButton(const String &name)
: Button(name),
displayText(name)
{}

void PowerFelCategoryButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) {
    const auto& bounds = pillBounds;
    float borderThick = 4.0f;
    float radius = float(bounds.getHeight()) / 2.0f;
    
    g.setColour(Colours::white);
    if (isEnabled()) {
        g.drawRoundedRectangle(bounds.getX() + borderThick, bounds.getY() + borderThick,
                               bounds.getWidth() - 2*borderThick, bounds.getHeight()  - 2*borderThick,
                               radius, borderThick);
    }
    
    // TODO: write button text as grey if choice is completely unset?
    g.setFont(20);
    g.drawText(displayText, bounds.getX(), bounds.getY(),
               bounds.getWidth(), bounds.getHeight(),
               Justification::centred);
}

void PowerFelCategoryButton::resized() {
    pillBounds.setSize(getLocalBounds().getWidth(), 42);
    fitRectInRect(pillBounds, getLocalBounds(), Justification::centred, false);
}

void PowerFelCategoryButton::setText(const String &text) {
    displayText = text;
    repaint();
}

PowerFelCategoryItemComponent::PowerFelCategoryItemComponent(const String &name)
: icon{ new DrawableButton("icon", DrawableButton::ImageFitted) },
button{ new PowerFelCategoryButton(name) } {
    addAndMakeVisible(icon);
    addAndMakeVisible(button);
    button->setEnabled(false); // default to disabled state
}

void PowerFelCategoryItemComponent::paint(Graphics &g) {}

void PowerFelCategoryItemComponent::resized() {
    auto b = getLocalBounds();
    auto h = b.getHeight();
    
    int spacing = 10;
    int togWidth = h * 1.1f;
    
    layout.setItemLayout(0, h, h, h);
    layout.setItemLayout(1, spacing, spacing, spacing);
    layout.setItemLayout(2, togWidth, togWidth, togWidth);
    layout.setItemLayout(3, spacing, spacing, spacing);
    layout.setItemLayout(4, h, -1, -1);
    
    Component *comps[] = { icon, nullptr, nullptr, button };
    layout.layOutComponents(comps, 5, b.getX(), b.getY(), b.getWidth(), b.getHeight(), false, true);
}

void PowerFelCategoryItemComponent::buttonClicked(Button *b) {}

void PowerFelCategoryItemComponent::buttonStateChanged(Button *b) {
    //
}

void PowerFelCategoryItemComponent::enablementChanged() {
    updateButtonText();
}

PowerFelPageComponent::PowerFelPageComponent() {
    bgColor = Colours::black;
    bgImage = "powerMenuBackground.png";
    mainPage = new Component();
    addAndMakeVisible(mainPage);
    mainPage->toBack();
    ChildProcess child{};
    debounce = 0;
    
    yesButton = new TextButton("yes");
    yesButton->setButtonText("yes");
    yesButton->addListener(this);
    addAndMakeVisible(yesButton);
    
    noButton = new TextButton("no");
    noButton->setButtonText("no");
    noButton->addListener(this);
    addAndMakeVisible(noButton);

}

PowerFelPageComponent::~PowerFelPageComponent() {}

void PowerFelPageComponent::paint(Graphics &g) {
    auto bounds = getLocalBounds();
    g.fillAll(bgColor);
    auto image = createImageFromFile(assetFile(bgImage));
    g.drawImage(image,bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 0, 0, image.getWidth(), image.getHeight(), false);
  
    g.setColour (Colours::white);
    g.setFont (22);
    g.drawText ("about to reboot into software flashing mode.", 0, 40, 480, 40, Justification::centred, true);
    g.drawText ("are you sure?", 0, 60, 480, 40, Justification::centred, true);
}

void PowerFelPageComponent::resized() {
    
    auto bounds = getLocalBounds();
    auto pageBounds = Rectangle<int>(120, 0, bounds.getWidth() - 120, bounds.getHeight());
  
    {
        for (int i = 0, j = 0; i < 4; ++i) {
            if (i > 0) verticalLayout.setItemLayout(j++, 0, -1, -1);
            verticalLayout.setItemLayout(j++, 48, 48, 48);
        }
        
        Component *PowerFelItems[] = { yesButton.get(), nullptr, nullptr, noButton.get() };
        auto b = bounds.reduced(10);
        b.setLeft(70);
        verticalLayout.layOutComponents(PowerFelItems, 1, b.getX(), b.getY(), b.getWidth(),
                                        b.getHeight(), true, true);
    }
    
    mainPage->setBounds(bounds);
    
    yesButton->setBounds(bounds.getWidth()/3.375, 120, 200, 40);
    noButton->setBounds(bounds.getWidth()/3.375, 185, 200, 40);
    //backButton->setBounds(bounds.getWidth()-60, bounds.getY(), 60, bounds.getHeight());
}

void PowerFelPageComponent::buttonStateChanged(Button *btn) {
  if (btn->isMouseButtonDown() && btn->isMouseOver()) {
    btn->setAlpha(0.5f);
  }
  else {
   btn->setAlpha(1.0f);
  }
}

void PowerFelPageComponent::buttonClicked(Button *button) {
      if (button == backButton || button == noButton ) {
        getMainStack().popPage(PageStackComponent::kTransitionTranslateHorizontalLeft);
      } else if( button == yesButton && !debounce ) {
        debounce = 1;
        int file = i2c_dev_open( "/dev/i2c-0", 0x34 );
        if(file) {
            i2c_write_byte(file, 0x4, 'f');
            i2c_write_byte(file, 0x5, 'b');
            i2c_write_byte(file, 0x6, '0');
            i2c_write_byte(file, 0x7, 0x0);
            child.start("systemctl reboot");
        }
      }
  }