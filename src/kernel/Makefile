include ../make.config

SHORT_VERSION=4
LONG_VERSION=$(SHORT_VERSION).7.2

all: fetch extract config build

linux-${VERSION}.tar.xz:
	-wget -c "https://www.kernel.org/pub/linux/kernel/v$(SHORT_VERSION).x/linux-${LONG_VERSION}.tar.xz"

linux-${VERSION}:
	-tar xf linux-${VERSION}.tar.xz

linux-${VERSION}/.config:
	-cp config-linux-${VERSION} linux-${VERSION}/.config && cd linux-${VERSION} && make oldconfig

build:
	-cd linux-${VERSION} && make -j8

install:
	-cd linux-${VERSION} && make install

fetch: linux-${VERSION}.tar.xz
extract: linux-${VERSION}
config: linux-${VERSION}/.config
clean:
	-rm -fr linux-${VERSION}*

.PHONY: all fetch extract config build install clean
