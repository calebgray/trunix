all: fetch extract build install

fetch:
	wget -c "http://ftp.gnu.org/gnu/bash/bash-4.2.53.tar.gz"

extract:
	tar xzf bash-4.2.53.tar.gz

build:
	bash -c "cd bash-4.2.53 && ./configure --enable-static-link --without-bash-malloc --prefix=$(shell realpath $(CURDIR)/../build) && make -j6"

install:
	bash -c "cd bash-4.2.53 && make install"

clean:
	rm -fr bash-4.2.53*
