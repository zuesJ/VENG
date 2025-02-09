CC = gcc
CFLAGS = -Wall -Wextra -fPIC

# Source and object files
SRC = src/VENG.c src/VENG_listeners.c src/VENG_tools.c
OBJ = $(SRC:.c=.o)

LIBRARY = libVENG.a

LIBS  = -lSDL2 -lSDL2_image -lSDL2_ttf -lm

# Targets
all: $(LIBRARY)

$(LIBRARY): $(OBJ)
	ar rcs $(LIBRARY) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	rm -f $(OBJ) $(LIBRARY)

