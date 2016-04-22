export CONFIG:=Release

PKG_CONFIG_PACKAGES = \
  NetworkManager \
  libnm-glib \

export PKG_CONFIG_CFLAGS=$(foreach pkg, $(PKG_CONFIG_PACKAGES), $(shell pkg-config --cflags $(pkg)))
export PKG_CONFIG_LDFLAGS=$(foreach pkg, $(PKG_CONFIG_PACKAGES), $(shell pkg-config --libs $(pkg)))



all:
	cd Builds/LinuxMakefile && $(MAKE)

clean:
	cd Builds/LinuxMakefile && $(MAKE) clean

wifitest:
	cd Builds/LinuxMakefile && $(MAKE) -f UnitTests.mk ../../build/$(CONFIG)/wifitest
