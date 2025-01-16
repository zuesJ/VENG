// Standard libs
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Debuging tools
//#define NDEBUG // Delete the comment to prevent assert() from checking.
#include <assert.h>

// Checking tools
#define IS_NULL(ptr) ((ptr) != NULL ? (ptr) : (printf("Pointer %s is NULL in line %d, %s.\n", #ptr, __LINE__, __FILE__), exit(1), NULL))

// SDL2 lib
//#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2_image/SDL_image.h>

// VENG
#include "VENG.h"

// main header
#include "main.h"

// Settings
#define TITLE "Physics Simulator"
#define FPS 144

SDL_Window* window;

SDL_Renderer* renderer;

VENG_Driver driver;

// main loop
int main (int argc, char* argv[])
{
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		printf("The system has failed to initialize the image subsystem: %s\n", IMG_GetError());
		exit(-1);
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("The system has failed to initialize the subsystems: %s\n", SDL_GetError());
		exit(-1);	
	}

	window = SDL_CreateWindow("VENG", SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED,
						1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	if (window == NULL)
	{
		printf("The system has failed to initialize the window: %s\n", SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL)
	{
		printf("The system has failed to initialize the window: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		exit(-1);
	}

	driver = (VENG_Driver){window, renderer};

	// Screen design
	VENG_Screen main_screen;
		VENG_Element** main_screen_elements = IS_NULL(calloc(2, sizeof(VENG_Element*)));
		
		VENG_Element canvas;
			main_screen_elements[0] = &canvas;

		VENG_Element tool_box;
			main_screen_elements[1] = &tool_box;

			VENG_Element** main_screen_tb_e = IS_NULL(calloc(2, sizeof(VENG_Element*)));

			VENG_Element box1;
				main_screen_tb_e[0] = &box1;

			VENG_Element box2;
				main_screen_tb_e[1] = &box2;


	main_screen = VENG_CreateScreen(TITLE, VENG_LoadPNG("res/Icon.png"), VENG_CreateLayout(HORIZONTAL, LEFT, TOP, main_screen_elements, 2));

	canvas = VENG_CreateElement(0.4f, 0.3f, true, true, VENG_CreateLayout(HORIZONTAL, LEFT, TOP, NULL, 0));

	tool_box = VENG_CreateElement(0.3f, 0.3f, true, true, VENG_CreateLayout(HORIZONTAL, RIGHT, BOTTOM, main_screen_tb_e, 2));

	box1 = VENG_CreateElement(0.2f, 0.3f, true, true, VENG_CreateLayout(HORIZONTAL, LEFT, TOP, NULL, 0));

	box2 = VENG_CreateElement(0.2f, 0.2f, true, true, VENG_CreateLayout(HORIZONTAL, LEFT, TOP, NULL, 0));


	VENG_Init(driver);
	VENG_SetScreen(&main_screen);

	SDL_Event event;
	int close_requested = 0;

	Uint64 time0 = SDL_GetTicks64();
	int w;
	int h;
	while(!close_requested)
	{
		while (SDL_PollEvent(&event)) // Retrieve events
		{
			if (event.type == SDL_QUIT)
			{
				close_requested = 1;
			}
		}
		
		SDL_RenderClear(renderer);

		VENG_PrepareElements();
		
		SDL_GetRendererOutputSize(renderer, &w, &h);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &canvas.rect);
		SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);
		SDL_RenderFillRect(renderer, &tool_box.rect);
	
		SDL_SetRenderDrawColor(renderer, 232, 18, 104, 255);
		SDL_RenderFillRect(renderer, &box1.rect);
		SDL_SetRenderDrawColor(renderer, 255, 128, 54, 255);
		SDL_RenderFillRect(renderer, &box2.rect);
	
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		SDL_RenderPresent(renderer);
		
		// FPS management
		int time = (1000 / FPS) - (SDL_GetTicks64() - time0);
		if (time < 0)
		{
			time = 0;
		}
		SDL_Delay(time);

		time0 = SDL_GetTicks64();
	}
	VENG_Destroy(true);
}
