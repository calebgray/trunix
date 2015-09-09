all: get extract build install

get:
	wget -c "http://www.nano-editor.org/dist/v2.4/nano-2.4.2.tar.gz"

extract:
	tar xzf nano-2.4.2.tar.gz

build:
	bash -c "cd /home/calebg/Projects/trunix/src/nano/nano-2.4.2 && ./configure --prefix=/home/calebg/Projects/trunix && make -j6 && make install"

install:
	bash -c "cd /home/calebg/Projects/trunix/src/trunix && gunzip ../initrd.img.gz && mkdir -p initrd && mount -o loop ../initrd.img initrd && mkdir -p initrd/bin && cp ../../bin/nano initrd/bin/nano && umount initrd && rmdir initrd && gzip ../initrd.img"
