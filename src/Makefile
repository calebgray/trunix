all:
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE)); done

initrd:
	-cd build && rm -f initrd.cpio && find . -depth -print | cpio -H newc -o > ../initrd.cpio && mv ../initrd.cpio .

install:
	-cd kernel && $(MAKE) install

clean:
	-rm -fr build
	-for DIR in */Makefile; do (cd $(CURDIR)/`dirname $$DIR`; $(MAKE) clean); done
