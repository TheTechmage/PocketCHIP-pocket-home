include Makefile

WifiTest_TARGET := wifitest
WifiTest_BLDCMD = $(CXX) -o $(OUTDIR)/$(WifiTest_TARGET) $(WifiTest_OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
WifiTest_CLEANCMD = rm -rf $(OUTDIR)/$(WifiTest_TARGET)

WifiTest_OBJECTS := \
  $(OBJDIR)/WifiStatus_30949170.o \
  $(OBJDIR)/juce_core_e13be5a9.o \
  $(OBJDIR)/juce_data_structures_b87144a5.o \
  $(OBJDIR)/juce_events_8da1559d.o \
  $(OBJDIR)/juce_graphics_5b381e81.o \
  $(OBJDIR)/juce_gui_basics_aa965d7d.o \
  $(OBJDIR)/WifiTest_1459807182.o \

$(OUTDIR)/$(WifiTest_TARGET): $(WifiTest_OBJECTS) $(RESOURCES)
	@echo Linking wifitest
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(WifiTest_BLDCMD)

$(OUTDIR)/$(WifiTest_TARGET)-clean:
	@echo Cleaning wifitest
	@$(WiFiTest_CLEANCMD)
	@$(CLEANCMD)

$(OBJDIR)/WifiTest_1459807182.o: ../../Source/WifiTest.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling WifiTest.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"
