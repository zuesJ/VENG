SHELL = cmd.exe

CC = gcc
OUTPUT_NAME = VENG_dev.exe

BUILD_DIR = ./build/

OUTPUT_PATH = $(OUTPUT_NAME)

# SDL2 dependencies (Delete the -mwindows comment to execute it without the console)
SDL2_DEPENDENCIES = -l advapi32 -l gdi32 -l imm32 -l kernel32 -l msvcrt -l ole32 \
		    -l oleaut32 -l setupapi -l shell32 -l user32 -l winmm -l version #-mwindows

# Add WinMain dependency
WinMain = -l mingw32

# Other libraries
LIBS = -lm

# SDL2
SDL2 = -I C:/SDL2/include -L C:/SDL2/lib -l SDL2main -l SDL2 -l SDL2_image

all: create_build_dir $(OUTPUT_PATH)

MODULES = $(BUILD_DIR)/main.o $(BUILD_DIR)/VENG.o $(BUILD_DIR)/VENG_listeners.o $(BUILD_DIR)/VENG_tools.o

$(OUTPUT_PATH): src/main.c src/VENG.c src/VENG_listeners.c src/VENG_tools.c src/VENG.h
	$(CC) -c src/VENG.c -o $(BUILD_DIR)/VENG.o $(SDL2) $(LIBS)
	$(CC) -c src/VENG_listeners.c -o $(BUILD_DIR)/VENG_listeners.o $(SDL2) $(LIBS)
	$(CC) -c src/VENG_tools.c -o $(BUILD_DIR)/VENG_tools.o $(SDL2) $(LIBS)
	$(CC) -c src/main.c -o $(BUILD_DIR)/main.o $(SDL2) $(LIBS)

	$(CC) -static $(MODULES) -o $(OUTPUT_PATH) $(WinMain) $(LIBS) $(SDL2) $(SDL2_DEPENDENCIES)

create_build_dir:
	@if not exist "$(BUILD_DIR)" echo "Making build directory . . ."
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

clean:
	@echo "Cleaning build. . ."
	@if exist "$(BUILD_DIR)" rd /s /q "$(BUILD_DIR)"
	@if exist "$(OUTPUT_NAME)" del "$(OUTPUT_NAME)"
	

