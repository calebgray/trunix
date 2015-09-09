
all: make install

make:
	mkdir -p ../build/
	g++ -std=c++1y -pedantic -static init.cpp -o ../build/init

install:
	cp -r bin ../build/
	cp -r usr ../build/

clean:
	echo
