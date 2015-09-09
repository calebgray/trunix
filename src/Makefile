all:
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE)); done

initrd:
	dd if=/dev/zero of=initrd.img bs=1M count=128
	mkfs.ext2 initrd.img
	mkdir -p initrd
	mount -o loop initrd.img initrd
	cp -r build/* initrd/
	umount initrd
	rmdir initrd
	gzip -f initrd.img

install:
	cp initrd.img.gz /boot/

clean:
	rm -fr build

mrproper:
	rm -fr inird initrd.img.gz
