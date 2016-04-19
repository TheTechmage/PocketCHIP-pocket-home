# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := ../../build/Debug

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=0.0.1" -D "JUCE_APP_VERSION_HEX=0x1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../deps/JUCE/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -ldl -lfreetype -lpthread -lrt 

  TARGET := pocket-home
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := ../../build/Release

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=0.0.1" -D "JUCE_APP_VERSION_HEX=0x1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../deps/JUCE/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -O3
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -ldl -lfreetype -lpthread -lrt 

  TARGET := pocket-home
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

OBJECTS := \
  $(OBJDIR)/PowerPageFelComponent_e97c8ca5.o \
  $(OBJDIR)/LibraryPageComponent_6532e93c.o \
  $(OBJDIR)/PowerPageComponent_4a26c732.o \
  $(OBJDIR)/BatteryMonitor_37302b96.o \
  $(OBJDIR)/WifiStatusJson_715858f8.o \
  $(OBJDIR)/WifiStatusNM_92fce0ef.o \
  $(OBJDIR)/WifiStatus_30949170.o \
  $(OBJDIR)/LauncherComponent_8a29041e.o \
  $(OBJDIR)/PageStackComponent_8da9db4d.o \
  $(OBJDIR)/SwitchComponent_a904080a.o \
  $(OBJDIR)/Utils_e4b11b92.o \
  $(OBJDIR)/LauncherBarComponent_b69b173.o \
  $(OBJDIR)/AppsPageComponent_7872953d.o \
  $(OBJDIR)/TrainComponent_b5ba2cde.o \
  $(OBJDIR)/PokeLookAndFeel_55885106.o \
  $(OBJDIR)/MainComponent_a6ffb4a5.o \
  $(OBJDIR)/SettingsPageComponent_10c3e5ac.o \
  $(OBJDIR)/SettingsPageBluetoothComponent_e05cbdea.o \
  $(OBJDIR)/SettingsPageWifiComponent_10168397.o \
  $(OBJDIR)/IconSliderComponent_f4b4d044.o \
  $(OBJDIR)/Main_90ebc5c2.o \
  $(OBJDIR)/BinaryData_ce4232d4.o \
  $(OBJDIR)/juce_core_e13be5a9.o \
  $(OBJDIR)/juce_data_structures_b87144a5.o \
  $(OBJDIR)/juce_events_8da1559d.o \
  $(OBJDIR)/juce_graphics_5b381e81.o \
  $(OBJDIR)/juce_gui_basics_aa965d7d.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(RESOURCES)
	@echo Linking pocket-home
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning pocket-home
	@$(CLEANCMD)

strip:
	@echo Stripping pocket-home
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/PowerPageFelComponent_e97c8ca5.o: ../../Source/PowerPageFelComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PowerPageFelComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LibraryPageComponent_6532e93c.o: ../../Source/LibraryPageComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LibraryPageComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PowerPageComponent_4a26c732.o: ../../Source/PowerPageComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PowerPageComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BatteryMonitor_37302b96.o: ../../Source/BatteryMonitor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BatteryMonitor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/WifiStatusJson_715858f8.o: ../../Source/WifiStatusJson.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling WifiStatusJson.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/WifiStatusNM_92fce0ef.o: ../../Source/WifiStatusNM.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling WifiStatusNM.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/WifiStatus_30949170.o: ../../Source/WifiStatus.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling WifiStatus.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LauncherComponent_8a29041e.o: ../../Source/LauncherComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LauncherComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PageStackComponent_8da9db4d.o: ../../Source/PageStackComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PageStackComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SwitchComponent_a904080a.o: ../../Source/SwitchComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SwitchComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Utils_e4b11b92.o: ../../Source/Utils.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Utils.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LauncherBarComponent_b69b173.o: ../../Source/LauncherBarComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LauncherBarComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AppsPageComponent_7872953d.o: ../../Source/AppsPageComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AppsPageComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/TrainComponent_b5ba2cde.o: ../../Source/TrainComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling TrainComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PokeLookAndFeel_55885106.o: ../../Source/PokeLookAndFeel.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PokeLookAndFeel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MainComponent_a6ffb4a5.o: ../../Source/MainComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MainComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SettingsPageComponent_10c3e5ac.o: ../../Source/SettingsPageComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SettingsPageComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SettingsPageBluetoothComponent_e05cbdea.o: ../../Source/SettingsPageBluetoothComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SettingsPageBluetoothComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SettingsPageWifiComponent_10168397.o: ../../Source/SettingsPageWifiComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SettingsPageWifiComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/IconSliderComponent_f4b4d044.o: ../../Source/IconSliderComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling IconSliderComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Main.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_e13be5a9.o: ../../deps/JUCE/modules/juce_core/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_b87144a5.o: ../../deps/JUCE/modules/juce_data_structures/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_8da1559d.o: ../../deps/JUCE/modules/juce_events/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_5b381e81.o: ../../deps/JUCE/modules/juce_graphics/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_aa965d7d.o: ../../deps/JUCE/modules/juce_gui_basics/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
