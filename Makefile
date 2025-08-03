.PHONY: all build clear

all: build

build:
	@mkdir -p build
	@gcc -c src/VENG.c -o build/VENG.o -I include/
	@gcc -c src/VENG_listeners.c -o build/VENG_listeners.o -I include/
	@ar rcs build/libVENG.a build/VENG.o build/VENG_listeners.o
	
clear:
	@rm -rf build