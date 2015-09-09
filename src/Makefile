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
	mv initrd.img.gz build/

install:
	cp build/initrd.img.gz /boot/
	cd kernel && $(MAKE) install

clean:
	rm -fr build
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE) clean); done
