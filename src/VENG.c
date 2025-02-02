#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Debuging tools
//#define NDEBUG // Delete the comment to prevent assert() from checking.
#include <assert.h>

// Pointer safety macro
#define IS_NULL(ptr) ((ptr) != NULL ? (ptr) : (printf("Pointer %s is NULL in line %d, %s.\n", #ptr, __LINE__, __FILE__), exit(1), NULL))

// SDL2 lib
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "VENG.h"

static VENG_Driver driver;
static VENG_Screen* rendering_screen;

// TO DO
static SDL_Point last_screen_size;
static bool any_dirty_element;

inline SDL_Rect VENG_StartDrawing(VENG_Element* element)
{
	SDL_RenderSetViewport(driver.renderer, &element->rect);
	SDL_Rect drawing_area = (SDL_Rect){0, 0, element->rect.w, element->rect.h};
	return drawing_area;
}

inline void VENG_StopDrawing(SDL_Rect* viewport)
{
	SDL_RenderSetViewport(driver.renderer, viewport);
}

int VENG_Init (VENG_Driver new_driver)
{
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		printf("The system has failed to initialize the image subsystem: %s\n", IMG_GetError());
		return -1;
	}
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("The system has failed to initialize the subsystems: %s\n", SDL_GetError());
		return -1;
	}
	driver = new_driver;
}

void VENG_Destroy (bool closeSDL)
{
	driver = (VENG_Driver){NULL, NULL};
	rendering_screen = NULL;
	last_screen_size.x = 0;
	last_screen_size.y = 0;
	any_dirty_element = false;
	if (closeSDL)
	{
		IMG_Quit();
		SDL_DestroyRenderer(driver.renderer);
		SDL_DestroyWindow(driver.window);
		SDL_Quit();
	}
}

VENG_Layout VENG_CreateLayout(VENG_Arrangement arrangement, VENG_Align align_horizontal, VENG_Align align_vertical, VENG_Element** sub_elements, size_t sub_elements_size)
{
	VENG_Layout layout;
	layout.arrangement = arrangement;
	layout.align_horizontal = align_horizontal;
	layout.align_vertical = align_vertical;
	layout.sub_elements = sub_elements;
	layout.sub_elements_size = sub_elements_size;
	return layout;
}

VENG_Screen VENG_CreateScreen(char* title, SDL_Surface* icon, VENG_Layout layout)
{
	VENG_Screen screen;
	screen.type = VENG_TYPE_SCREEN;
	screen.title = title;
	screen.icon = icon;
	screen.layout = layout;
	return screen;
}

VENG_Element VENG_CreateElement(float w, float h, bool stretch_size, bool visible, VENG_Layout layout)
{
	VENG_Element element;
	element.type = VENG_TYPE_ELEMENT;
	element.w = w;
	element.h = h;
	element.stretch_size = stretch_size;
	element.visible = visible;
	element.layout = layout;
	return element;
}

void VENG_SetScreen(VENG_Screen* screen)
{
	if (screen == NULL)
	{
		return;
	}
	rendering_screen = screen;
	if (screen->icon != NULL)
	{
		SDL_SetWindowIcon(driver.window, screen->icon);
	}
	SDL_SetWindowTitle(driver.window, screen->title);
}

void VENG_PrepareElements()
{
	if (rendering_screen == NULL)
	{
		return;
	}
	if (rendering_screen->layout.sub_elements == NULL)
	{
		return;
	}

	int window_w;
	int window_h;
	SDL_GetRendererOutputSize(driver.renderer, &window_w, &window_h);

	for (int i = 0; i < rendering_screen->layout.sub_elements_size; i++)
	{
		if (rendering_screen->layout.sub_elements[i] != NULL)
		{
			VENG_PrepareElement(rendering_screen->layout.sub_elements[i], rendering_screen, (SDL_Rect){0, 0, window_w, window_h});
		}
	}
}

void VENG_PrepareElement(VENG_Element* element, void* parent_container, SDL_Rect drawing_rect)
{
	if (!element->visible)
	{
		// Sets the rect with -1 to avoid SDL to render the element
		element->rect = (SDL_Rect){-1, -1, -1, -1};
		return;
	}

	// Detect if the parent_container is a Screen* or an Element*
	VENG_ParentType type = ((VENG_Screen*)parent_container)->type;
	VENG_Layout* layout;
	if (type == VENG_TYPE_SCREEN)
	{
		layout = &((VENG_Screen*)parent_container)->layout;
	}
	else
	{
		layout = &((VENG_Element*) parent_container)->layout;
	}
	

	// Compute Size
	float screen_ratio = (float)drawing_rect.w / drawing_rect.h;
	if (element->stretch_size)
	{
		element->rect.w = round(element->w * drawing_rect.w);
		element->rect.h = round(element->h * drawing_rect.h);
	}
	else
	{
		if (screen_ratio >= 1.0f)	
		{
			element->rect.w = round(element->w * drawing_rect.h);
			element->rect.h = round(element->h * drawing_rect.h);
		}
		else
		{
			element->rect.w = round(element->w * drawing_rect.w);
			element->rect.h = round(element->h * drawing_rect.w);
		}
	}

	// Compute coords
	SDL_Point offset = (SDL_Point){0, 0};
	for (int i = 0; i < layout->sub_elements_size; i++)
	{
		if (layout->sub_elements[i] == element)
		{
			break;
		}
		if (element->stretch_size)
		{
			offset.x += round(element->w * drawing_rect.w);
			offset.y += round(element->h * drawing_rect.h);
		}
		else
		{
			if (screen_ratio >= 1.0f)	
			{
				offset.x += round(element->w * drawing_rect.h);
				offset.y += round(element->h * drawing_rect.h);
			}
			else
			{
				offset.x += round(element->w * drawing_rect.w);
				offset.y += round(element->h * drawing_rect.w);
			}
		}
	}
	// Set the coords
	if (layout->arrangement == VENG_HORIZONTAL)
	{
		switch (layout->align_horizontal)
		{
			case VENG_LEFT:
				element->rect.x = drawing_rect.x + offset.x;
				break;
			case VENG_CENTER:
				element->rect.x = drawing_rect.x + round((drawing_rect.w - element->rect.w)/2); // some offset
				break;
			case VENG_RIGHT:
				element->rect.x = drawing_rect.x + drawing_rect.w - element->rect.w - offset.x;
				break;
			default:
				printf("Invalid align_h argument.");
				exit(-1);
		}
		switch (layout->align_vertical)
		{
			case VENG_TOP:
				element->rect.y = drawing_rect.y;
				break;
			case VENG_CENTER:
				element->rect.y = drawing_rect.y + round((drawing_rect.h - element->rect.h)/2);
				break;
			case VENG_BOTTOM:
				element->rect.y = drawing_rect.y + drawing_rect.h - element->rect.h;
				break;
			default:
				printf("Invalid align_v argument.");
				exit(-1);
		}
	}
	else if (layout->arrangement == VENG_VERTICAL)
	{
		switch (layout->align_horizontal)
		{
			case VENG_LEFT:
				element->rect.x = drawing_rect.x;
				break;
			case VENG_CENTER:
				element->rect.x = drawing_rect.x + round((drawing_rect.w - element->rect.w)/2);
				break;
			case VENG_RIGHT:
				element->rect.x = drawing_rect.x + drawing_rect.w - element->rect.w;
				break;
			default:
				printf("Invalid align_h argument.");
				exit(-1);
		}
		switch (layout->align_vertical)
		{
			case VENG_TOP:
				element->rect.y = drawing_rect.y + offset.y;
				break;
			case VENG_CENTER:
				element->rect.y = drawing_rect.y + round((drawing_rect.h - element->rect.h)/2); // + some h offset
				break;
			case VENG_BOTTOM:
				element->rect.y = drawing_rect.y + drawing_rect.h - element->rect.h - offset.y;
				break;
			default:
				printf("Invalid align_v argument.");
				exit(-1);
		}
	}
	else
	{
		printf("Invalid arrangement argument.");
		exit(-1);
	}

	// Check for sub-elements

	if (element->layout.sub_elements != NULL)
	{
		for (int i = 0; i < element->layout.sub_elements_size; i++)
		{
			if (element->layout.sub_elements[i] != NULL)
			{
				VENG_PrepareElement(element->layout.sub_elements[i], element, element->rect);
			}

		}
	}
}

VENG_Screen* VENG_GetScreen()
{
	return rendering_screen;
}

VENG_Driver VENG_GetDriver ()
{
	return driver;
}

VENG_Driver VENG_CreateDriver(SDL_Window* window, SDL_Renderer* renderer)
{
	VENG_Driver driver;
	driver.window = window;
	driver.renderer = renderer;
	return driver;
}

void VENG_SetDriver(VENG_Driver new_driver)
{
	driver = new_driver;
}
