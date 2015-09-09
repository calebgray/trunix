all: get extract build install

get:
	wget -c "http://ftp.gnu.org/gnu/bash/bash-4.2.53.tar.gz"

extract:
	tar xzf bash-4.2.53.tar.gz

build:
	bash -c "cd /home/calebg/Projects/trunix/src/bash/bash-4.2.53 && ./configure --without-bash-malloc --prefix=/home/calebg/Projects/trunix && make -j6 && make install"

build-static:
	bash -c "cd /home/calebg/Projects/trunix/src/bash/bash-4.2.53 && ./configure --enable-static-link --without-bash-malloc --prefix=/home/calebg/Projects/trunix && make -j6 && make install"

install:
	bash -c "cd /home/calebg/Projects/trunix/src/trunix && gunzip ../initrd.img.gz && mkdir -p initrd && mount -o loop ../initrd.img initrd && mkdir -p initrd/bin && cp ../../bin/bash initrd/bin/bash && umount initrd && rmdir initrd && gzip ../initrd.img"
