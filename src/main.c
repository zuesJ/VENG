// Standard libs
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>

#include "VENG.h"

#include "main.h"

// Settings
#define TITLE "Physics Simulator"
#define FPS 5

static void start_SDL();

SDL_Window* window;
SDL_Renderer* renderer;
VENG_Driver driver;

int main (int argc, char* argv[])
{
	start_SDL();

	VENG_Init(driver);
	
	VENG_Screen* main_screen = VENG_CreateScreen(TITLE, VENG_LoadPNG("res/Icon.png"), VENG_CreateLayout(VENG_HORIZONTAL, VENG_RIGHT, VENG_BOTTOM, 3));
	VENG_Element* element = VENG_CreateElement(0.5f, 0.5f, true, true, VENG_CreateLayout(VENG_HORIZONTAL, VENG_LEFT, VENG_TOP, 1));
	VENG_Element* element2 = VENG_CreateElement(0.5f, 0.3f, false, true, VENG_CreateLayout(VENG_HORIZONTAL, VENG_CENTER, VENG_CENTER, 0));
	VENG_Element* sub_element = VENG_CreateElement(0.2f, 0.2f, true, true, VENG_CreateLayout(VENG_HORIZONTAL, VENG_LEFT, VENG_TOP, 1));
	VENG_Element* sub_element2 = VENG_CreateElement(0.2f, 0.2f, true, true, VENG_CreateLayout(VENG_HORIZONTAL, VENG_LEFT, VENG_TOP, 0));
	VENG_Element* test = VENG_CreateElement(0.2f, 0.3f, false, true, VENG_CreateLayout(VENG_HORIZONTAL, VENG_LEFT, VENG_TOP, 0));

	VENG_AddElementToScreen(element, main_screen);
	VENG_AddElementToScreen(element2, main_screen);
	VENG_AddElementToScreen(test, main_screen);
	VENG_AddSubElementToElement(sub_element, element);
	VENG_AddSubElementToElement(sub_element2, sub_element);
	VENG_SetScreen(main_screen);

	VENG_PrintScreenHierarchy(main_screen);

	SDL_Event event;
	int close_requested = 0;

	Uint64 time0 = SDL_GetTicks64();
	int w;
	int h;
	while(!close_requested)
	{
		Uint64 start = SDL_GetPerformanceCounter();

		while (SDL_PollEvent(&event))
		{	
			VENG_Listen(event);
			switch (event.type)
			{
				case SDL_QUIT:
					close_requested = 1;
					break;
			}
		}

		SDL_RenderClear(renderer);

		VENG_PrepareScreen(main_screen);
		VENG_PrintScreenHierarchy(main_screen);

		fill_a_rect_with_color(element, (SDL_Color){255, 165, 100, 255});
		fill_a_rect_with_color(element2, (SDL_Color){255, 12, 100, 255});
		fill_a_rect_with_color(sub_element, (SDL_Color){255, 255, 255, 255});
		fill_a_rect_with_color(sub_element2, (SDL_Color){255, 13, 67, 255});
		fill_a_rect_with_color(test, (SDL_Color){232, 13, 67, 255});


		SDL_GetRendererOutputSize(renderer, &w, &h);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		SDL_RenderPresent(renderer);
		
		Uint64 end = SDL_GetPerformanceCounter();

		double elapsed = (double)(end - start) / SDL_GetPerformanceFrequency();

    	//printf("Time taken: %f seconds\n", elapsed);

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

static void start_SDL()
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
}