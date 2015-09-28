VERSION=4.2.1

all: fetch extract config build

fetch:
	wget -c "https://www.kernel.org/pub/linux/kernel/v4.x/linux-${VERSION}.tar.xz"

extract:
	tar xf linux-${VERSION}.tar.xz

config:
	-cp config-linux-${VERSION} linux-${VERSION}/.config && cd linux-${VERSION} && make oldconfig

build:
	-cd linux-${VERSION} && make -j6

install:
	-cd linux-${VERSION} && make install

clean:
	rm -fr linux-${VERSION}*
