CONFIG:=Release

all:
	cd Builds/LinuxMakefile && CONFIG=$(CONFIG) make

clean:
	cd Builds/LinuxMakefile && CONFIG=$(CONFIG) make clean

wifitest:
	cd Builds/LinuxMakefile && CONFIG=$(CONFIG) make -f UnitTests.mk ../../build/$(CONFIG)/wifitest
