
all: make install

initrd:
	dd if=/dev/zero of=../initrd.img bs=1M count=128
	mkfs.ext2 ../initrd.img
	gzip -f ../initrd.img

make: make-init

make-init:
	mkdir -p build/initrd
	g++ -std=c++1y -pedantic -static src/initrd/init.cpp -o build/initrd/init

install:
	gunzip ../initrd.img.gz
	mkdir -p initrd
	mount -o loop ../initrd.img initrd
	cp build/initrd/init initrd/init
	cp -r src/initrd/bin initrd/
	cp -r src/initrd/usr initrd/
	umount initrd
	rmdir initrd
	gzip -f ../initrd.img

clean:
	rm -fr build

mrproper:
	rm ../initrd.img.gz
