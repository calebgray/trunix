all: fetch extract build

fetch:
	wget -c "http://ftp.gnu.org/gnu/bash/bash-4.2.53.tar.gz"

extract:
	tar xzf bash-4.2.53.tar.gz

build:
	BUILD_DIR=$(realpath `pwd`/../../build)
	bash -c "cd /home/calebg/Projects/trunix/src/bash/bash-4.2.53 && ./configure --enable-static-link --without-bash-malloc --prefix=$BUILD_DIR && make -j6 && make install"
