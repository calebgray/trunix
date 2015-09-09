all: fetch extract config build

fetch:
	wget "https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.2.tar.xz"

extract:
	tar xf linux-4.2.tar.xz

config:
	bash -c "cd linux-4.2 && make oldconfig"

build:
	bash -c "cd linux-4.2 && make -j6"

install:
	echo "not yet."
