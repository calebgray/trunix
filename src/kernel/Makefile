all: fetch extract config build

fetch:
	wget -c "https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.2.tar.xz"

extract:
	tar xf linux-4.2.tar.xz

config:
	bash -c "cp .config linux-4.2 && cd linux-4.2 && make oldconfig"

build:
	bash -c "cd linux-4.2 && make -j6"

install:
	bash -c "cd linux-4.2 && make install"

clean:
	rm -fr linux-4.2*
