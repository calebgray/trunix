all:
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE)); done

initrd:
	dd if=/dev/zero of=build/initrd.img bs=1M count=128
	mkfs.ext2 build/initrd.img
	mkdir -p initrd
	mount -o loop build/initrd.img initrd
	cp -r build/* initrd/
	umount initrd
	rmdir initrd
	gzip -f build/initrd.img

install:
	cp build/initrd.img.gz /boot/
	cd kernel && $(MAKE) install

clean:
	rm -fr build
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE) clean); done
