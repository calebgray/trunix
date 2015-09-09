mount:
	mkdir -p initrd
	gunzip initrd.img.gz
	mount -o loop initrd.img initrd

umount:
	umount initrd
	gzip initrd.img
	rmdir initrd

install:
	cp initrd.img.gz /boot/initrd.img.gz
