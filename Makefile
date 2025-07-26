all: build

build: ./build
	mkdir build
	cd ./build && cmake .. && make
	
clear:
	rm -r build