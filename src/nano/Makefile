all: fetch extract build install

fetch:
	wget -c "http://www.nano-editor.org/dist/v2.4/nano-2.4.2.tar.gz"

extract:
	tar xzf nano-2.4.2.tar.gz

build:
	bash -c "cd nano-2.4.2 && CFLAGS="-static" ./configure --prefix=$(shell realpath $(CURDIR)/../build) --disable-mouse && make -j6"

install:
	bash -c "cd nano-2.4.2 && make install"

clean:
	rm -fr nano-2.4.2*
