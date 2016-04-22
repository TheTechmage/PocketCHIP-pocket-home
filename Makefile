CONFIG:=Release

all:
	cd Builds/LinuxMakefile && CONFIG=$(CONFIG) $(MAKE)

clean:
	cd Builds/LinuxMakefile && CONFIG=$(CONFIG) $(MAKE) clean

wifitest:
	cd Builds/LinuxMakefile && CONFIG=$(CONFIG) $(MAKE) -f UnitTests.mk ../../build/$(CONFIG)/wifitest
