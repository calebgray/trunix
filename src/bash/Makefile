include ../make.config

VERSION=4.4-rc2

test:
	echo $(BUILD_DIR)

all: fetch extract config build install

bash-$(VERSION).tar.gz:
	-wget -c "http://ftp.gnu.org/gnu/bash/bash-$(VERSION).tar.gz"

bash-$(VERSION): bash-$(VERSION).tar.gz
	-tar xzf bash-$(VERSION).tar.gz

bash-$(VERSION)/bash: bash-$(VERSION)
	-cd bash-$(VERSION) && ./configure --enable-static-link --without-bash-malloc --prefix=$(BUILD_DIR) && make -j8

$(BUILD_DIR)/bin/bash: bash-$(VERSION)/bash
	-cd bash-$(VERSION) && make install

fetch: bash-$(VERSION).tar.gz
extract: bash-$(VERSION)
config:
build: bash-$(VERSION)/bash
install: $(BUILD_DIR)/bin/bash
clean:
	rm -fr bash-$(VERSION)*

.PHONY: all fetch extract build install clean
