all:
	cd Builds/LinuxMakefile && CONFIG=Release make

clean:
	cd Builds/LinuxMakefile && CONFIG=Release make clean

wifitest:
	cd Builds/LinuxMakefile && CONFIG=Release make -f UnitTests.mk ../../build/Release/wifitest
