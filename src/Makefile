include ./make.config

test:
	-cd bash && $(MAKE)

all:
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE)); done

initrd:
	-cd build && rm -f initrd.cpio.gz && find . -depth -print | cpio -H newc -o > ../initrd.cpio && mv ../initrd.cpio . && gzip initrd.cpio

install:
	-cd kernel && $(MAKE) install
	-cp build/initrd.cpio.gz /boot/

clean:
	-rm -fr build
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE) clean); done
