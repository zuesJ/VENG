#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface* VENG_LoadPNG (const char* path)
{
	SDL_Surface* surface = IMG_Load(path);
	if (surface == NULL)
	{
		printf("Failed to load the PNG image: %s\n", IMG_GetError());
		return NULL;
	}
	return surface;
}
