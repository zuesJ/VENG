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

// main loop
int main (int argc, char* argv[])
{
	VENG_Screen main_screen = VENG_Screen_Create(TITLE, LoadPNG("res/Icon.png"), VENG_Layout_Create(HORIZONTAL, LEFT, TOP, NULL, 2));
	VENG_Element** main_screen_elements = IS_NULL(calloc(2, sizeof(VENG_Element*)));

		VENG_Element canvas = VENG_Element_Create(0.4f, 0.3f, true, true, VENG_Layout_Create(HORIZONTAL, LEFT, TOP, NULL, 0));
		main_screen_elements[0] = &canvas;

		VENG_Element tool_box = VENG_Element_Create(0.3f, 0.3f, true, true, VENG_Layout_Create(HORIZONTAL, RIGHT, BOTTOM, NULL, 2));
		
			VENG_Element** main_screen_tb_e = IS_NULL(calloc(2, sizeof(VENG_Element*)));
			tool_box.layout.sub_elements = main_screen_tb_e;
				VENG_Element box1 = VENG_Element_Create(0.2f, 0.3f, true, true, VENG_Layout_Create(HORIZONTAL, LEFT, TOP, NULL, 0));
				main_screen_tb_e[0] = &box1;

				VENG_Element box2 = VENG_Element_Create(0.2f, 0.2f, true, true, VENG_Layout_Create(HORIZONTAL, LEFT, TOP, NULL, 0));
				main_screen_tb_e[1] = &box2;
		main_screen_elements[1] = &tool_box;
		
		
	main_screen.layout.sub_elements = main_screen_elements;

	VENG_Init(&main_screen);

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
		
		SDL_RenderClear(VENG_GetRenderer());

		VENG_PrepareElements();
		
		SDL_GetRendererOutputSize(VENG_GetRenderer(), &w, &h);

		SDL_SetRenderDrawColor(VENG_GetRenderer(), 255, 255, 255, 255);
		SDL_RenderFillRect(VENG_GetRenderer(), &canvas.rect);
		SDL_SetRenderDrawColor(VENG_GetRenderer(), 100, 255, 255, 255);
		SDL_RenderFillRect(VENG_GetRenderer(), &tool_box.rect);
	
		SDL_SetRenderDrawColor(VENG_GetRenderer(), 232, 18, 104, 255);
		SDL_RenderFillRect(VENG_GetRenderer(), &box1.rect);
		SDL_SetRenderDrawColor(VENG_GetRenderer(), 255, 128, 54, 255);
		SDL_RenderFillRect(VENG_GetRenderer(), &box2.rect);
	
		SDL_SetRenderDrawColor(VENG_GetRenderer(), 0, 0, 0, 255);

		SDL_RenderPresent(VENG_GetRenderer());
		
		// FPS management
		int time = (1000 / FPS) - (SDL_GetTicks64() - time0);
		if (time < 0)
		{
			time = 0;
		}
		SDL_Delay(time);

		time0 = SDL_GetTicks64();
	}
	VENG_Destroy();
}
