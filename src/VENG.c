#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Pointer safety macro
#define IS_NULL(ptr) ((ptr) != NULL ? (ptr) : (printf("Pointer %s is NULL in line %d, %s.\n", #ptr, __LINE__, __FILE__), exit(1), NULL))

// SDL2 lib
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "VENG.h"

static bool started = false;

static VENG_Driver driver;
static VENG_Screen* rendering_screen;

#define ALLOCATED_SCREENS_START 10
static VENG_Screen** screens = NULL;
static size_t screens_current_slots = ALLOCATED_SCREENS_START;
static size_t screens_used_slots = 0;

#define ALLOCATED_ELEMENTS_START 100
static VENG_Element** elements = NULL;
static size_t elements_current_slots = ALLOCATED_ELEMENTS_START;
static size_t elements_used_slots = 0;

// TO DO
static SDL_Point last_screen_size;
static bool any_dirty_element;

static void __PrintElementHierarchy(VENG_Element* element, size_t tabs);
void VENG_PrintScreenHierarchy(VENG_Screen* screen)
{
	printf("Screen: %s\n", screen->title);
	printf("\tAttributes: isDirty: %d, Arrangement: %d, align_h: %d, align_v: %d\n",
			screen->dirty, screen->layout.arrangement, screen->layout.align_horizontal, screen->layout.align_vertical);
	printf("\tSub_elements: %p, size: %ld, count: %ld\n" , screen->layout.sub_elements, screen->layout.sub_elements_size, screen->layout.sub_elements_count);
	for (size_t i = 0; i < screen->layout.sub_elements_size; i++)
	{
		if (screen->layout.sub_elements[i] == NULL)
		{
			printf("\t\tSlot %ld: empty\n", i);
		}
		else
		{
			printf("\t\tSlot %ld: ", i);
			__PrintElementHierarchy(screen->layout.sub_elements[i], 2);
		}
	}
}
static void __PrintElementHierarchy(VENG_Element* element, size_t tabs)
{
	printf("%p\n", element);
	for (size_t tab = 0; tab < tabs; tab++)
	{
		printf("\t");
	}
	printf("\tAttributes: Rect={%d, %d, %d, %d}, W,H: %.3f %.3f, stretch_size: %d, visible: %d, dirty: %d, Arrangement: %d, align_h: %d, align_v: %d\n", element->rect.x, element->rect.y, element->rect.w, element->rect.h,
			element->w, element->h, element->stretch_size, element->visible, element->dirty, element->layout.arrangement, element->layout.align_horizontal, element->layout.align_vertical);
	for (size_t tab = 0; tab < tabs; tab++)
	{
		printf("\t");
	}
	printf("\tSub_elements: %p, size: %ld, count: %ld\n" , element->layout.sub_elements, element->layout.sub_elements_size, element->layout.sub_elements_count);
	for (size_t i = 0; i < element->layout.sub_elements_size; i++)
	{
		if (element->layout.sub_elements == NULL)
		{
			for (; i < element->layout.sub_elements_size; i++)
			{
				for (size_t tab = 0; tab < tabs; tab++)
				{
					printf("\t");
				}
				printf("\t\tSlot %ld: empty\n", i);
				}
			break;
		}
		if (element->layout.sub_elements[i] == NULL)
		{
			for (size_t tab = 0; tab < tabs; tab++)
			{
				printf("\t");
			}
			printf("\t\tSlot %ld: empty\n", i);
		}
		else
		{
			for (size_t tab = 0; tab < tabs; tab++)
			{
				printf("\t");
			}
			printf("\t\tSlot %ld: ", i);
			__PrintElementHierarchy(element->layout.sub_elements[i], tabs+2);
		}
	}
}

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
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("The system has failed to initialize the subsystems: %s\n", SDL_GetError());
		return -1;
	}
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags)
	{
		printf("The system has failed to initialize the image subsystem: %s\n", IMG_GetError());
		return -1;
	}
	if (new_driver.window == NULL || new_driver.renderer == NULL)
	{
		printf("Driver contains NULL contents\n");
		return -1;
	}

	driver = new_driver;
	screens = IS_NULL(calloc(ALLOCATED_SCREENS_START, sizeof(VENG_Screen*)));
	elements = IS_NULL(calloc(ALLOCATED_ELEMENTS_START, sizeof(VENG_Element*)));
	started = true;
	return 0;
}

void VENG_Destroy (bool closeSDL)
{
	driver = (VENG_Driver){NULL, NULL};
	rendering_screen = NULL;
	last_screen_size.x = 0;
	last_screen_size.y = 0;
	any_dirty_element = false;
	VENG_ResetListeners();
	if (closeSDL)
	{
		IMG_Quit();
		SDL_DestroyRenderer(driver.renderer);
		SDL_DestroyWindow(driver.window);
		SDL_Quit();
	}
	started = false;
}

VENG_Layout VENG_CreateLayout(VENG_Arrangement arrangement, VENG_Align align_horizontal, VENG_Align align_vertical, size_t max_childs)
{
	VENG_Layout layout;
	layout.arrangement = arrangement;
	layout.align_horizontal = align_horizontal;
	layout.align_vertical = align_vertical;
	layout.sub_elements = NULL;
	layout.sub_elements_size = max_childs;

	return layout;
}

VENG_Screen* VENG_CreateScreen(char* title, SDL_Surface* icon, VENG_Layout layout)
{
	if (!started)
	{
		printf("VENG is not initialized yet\n");
		exit(EXIT_FAILURE);
	}

	if (screens_used_slots >= screens_current_slots)
	{
		screens_current_slots += ALLOCATED_SCREENS_START;
		screens = IS_NULL(realloc(screens, screens_current_slots));
		for (size_t i = screens_current_slots - ALLOCATED_SCREENS_START; i < screens_current_slots; i++)
		{
			screens[i] = NULL;
		}
	}
	VENG_Screen* return_adress;
	for (size_t i = 0; i < screens_current_slots; i++)
	{
		if (screens[i] == NULL)
		{
			screens[i] = IS_NULL(calloc(1, sizeof(VENG_Screen)));
			screens[i]->type = VENG_TYPE_SCREEN;
			screens[i]->title = title;
			screens[i]->icon = icon;
			screens[i]->layout = layout;
			screens[i]->dirty = true;
			return_adress = screens[i];
			break;
		}
	}
	screens_used_slots++;
	return return_adress;
}

VENG_Element* VENG_CreateElement(float w, float h, bool stretch_size, bool visible, VENG_Layout layout)
{
	if (!started)
	{
		printf("VENG is not initialized yet\n");
		exit(EXIT_FAILURE);
	}
	if (elements_used_slots >= elements_current_slots)
	{
		elements_current_slots += ALLOCATED_ELEMENTS_START;
		elements = IS_NULL(realloc(elements, elements_current_slots));
		for (size_t i = elements_current_slots - ALLOCATED_ELEMENTS_START; i < elements_current_slots; i++)
		{
			elements[i] = NULL;
		}
	}
	VENG_Element* return_adress;
	for (size_t i = 0; i < elements_current_slots; i++)
	{
		if (elements[i] == NULL)
		{
			elements[i] = IS_NULL(calloc(1, sizeof(VENG_Element)));
			elements[i]->type = VENG_TYPE_ELEMENT;
			elements[i]->w = w;
			elements[i]->h = h;
			elements[i]->stretch_size = stretch_size;
			elements[i]->visible = visible;
			elements[i]->layout = layout;
			elements[i]->layout.sub_elements = NULL;
			elements[i]->layout.sub_elements_count = 0;
			elements[i]->dirty = true;
			return_adress = elements[i];
			break;
		}
	}
	return return_adress;
}

void VENG_AddElementToScreen(VENG_Element* element, VENG_Screen* screen)
{
	if (!started)
	{
		printf("VENG is not initialized yet\n");
		exit(EXIT_FAILURE);
	}
	if (screen->layout.sub_elements == NULL)
	{
		screen->layout.sub_elements = IS_NULL(calloc(screen->layout.sub_elements_size, sizeof(VENG_Element*)));
		screen->layout.sub_elements_count = 0;
	}
	if (screen->layout.sub_elements_count >= screen->layout.sub_elements_size)
	{
		printf("Couldn't add Element: max size reached\n");
		return;
	}
	for (size_t i = 0; i < screen->layout.sub_elements_size; i++)
	{
		if (screen->layout.sub_elements[i] == NULL)
		{
			screen->layout.sub_elements[i] = element;
			screen->layout.sub_elements_count += 1;
			screen->dirty = true;
			break;
		}
	}
}

void VENG_AddSubElementToElement(VENG_Element* sub_element, VENG_Element* element)
{
	if (!started)
	{
		printf("VENG is not initialized yet\n");
		exit(EXIT_FAILURE);
	}
	if (element->layout.sub_elements == NULL)
	{
		element->layout.sub_elements = IS_NULL(calloc(element->layout.sub_elements_size, sizeof(VENG_Element*)));
		element->layout.sub_elements_count = 0;
	}
	if (element->layout.sub_elements_count >= element->layout.sub_elements_size)
	{
		printf("Couldn't add Element: max size reached\n");
		return;
	}
	for (size_t i = 0; i < element->layout.sub_elements_size; i++)
	{
		if (element->layout.sub_elements[i] == NULL)
		{
			element->layout.sub_elements[i] = sub_element;
			element->layout.sub_elements_count += 1;
			break;
		}
	}
}

void VENG_SetScreen(VENG_Screen* screen)
{
	if (screen == NULL)
	{
		return;
	}
	rendering_screen = screen;
	VENG_ResetListeners();
	if (screen->icon != NULL)
	{
		SDL_SetWindowIcon(driver.window, screen->icon);
	}
	SDL_SetWindowTitle(driver.window, screen->title);
}

void VENG_PrepareScreen(VENG_Screen* screen)
{
	if (screen == NULL)
	{
		return;
	}
	if (screen->layout.sub_elements == NULL || screen->layout.sub_elements_count == 0)
	{
		return;
	}

	int window_w;
	int window_h;
	SDL_GetRendererOutputSize(driver.renderer, &window_w, &window_h);
	VENG_PrepareChilds(screen, (SDL_Rect){0, 0, window_w, window_h});
}

void VENG_PrepareChilds(void* parent_container, SDL_Rect drawing_rect)
{
	// (I) Compute every child's size
	// (II) Once computed, align every child
	// (III) Check if the childs have more childs
	/*This is weird, isn't it?*/ 

	if (parent_container == NULL)
	{
		return;
	}

	VENG_Layout* layout = NULL;
	{
		VENG_Screen* s = (VENG_Screen*)parent_container;
		VENG_ParentType type = s->type;
		if (type == VENG_TYPE_SCREEN)
		{
			VENG_Screen* scr = (VENG_Screen*)parent_container;
			layout = &scr->layout;
		}
		else if (type == VENG_TYPE_ELEMENT)
		{
			VENG_Element* ele = (VENG_Element*)parent_container;
			layout = &ele->layout;
		}
		else
		{
			printf("Something went wrong. ID = 936183");
		}
	}

	if (layout == NULL)
	{
		return;
	}

	if (layout->sub_elements == NULL || layout->sub_elements_size == 0 || layout->sub_elements_count == 0)
	{
		return;
	}

	// (I)
	float screen_ratio = (float)drawing_rect.w / drawing_rect.h;
	for (size_t i = 0; i < layout->sub_elements_size; i++)
	{
		if (layout->sub_elements[i] != NULL)
		{
			if (!layout->sub_elements[i]->visible)
			{
				layout->sub_elements[i]->rect = (SDL_Rect){-1, -1, -1, -1};
				continue;
			}
			if (layout->sub_elements[i]->stretch_size)
			{
				layout->sub_elements[i]->rect.w = round(layout->sub_elements[i]->w * drawing_rect.w);
				layout->sub_elements[i]->rect.h = round(layout->sub_elements[i]->h * drawing_rect.h);
			}
			else
			{
				if (screen_ratio >= 1.0f)	
				{
					layout->sub_elements[i]->rect.w = round(layout->sub_elements[i]->w * drawing_rect.h);
					layout->sub_elements[i]->rect.h = round(layout->sub_elements[i]->h * drawing_rect.h);
				}
				else
				{
					layout->sub_elements[i]->rect.w = round(layout->sub_elements[i]->w * drawing_rect.w);
					layout->sub_elements[i]->rect.h = round(layout->sub_elements[i]->h * drawing_rect.w);
				}
			}
		}
	}

	// (II)
	SDL_Point offset = (SDL_Point){0, 0};
	SDL_Point current_center = (SDL_Point){0, 0};
	if (layout->align_horizontal == VENG_CENTER)
	{
		for (size_t i = 0; i < layout->sub_elements_size; i++)
		{
			if (layout->sub_elements[i] != NULL)
			{
				current_center.x += layout->sub_elements[i]->rect.w;
			}
		}
		current_center.x = round((drawing_rect.w - current_center.x) / 2);
	}
	if (layout->align_vertical == VENG_CENTER)
	{
		for (size_t i = 0; i < layout->sub_elements_size; i++)
		{
			if (layout->sub_elements[i] != NULL)
			{
				current_center.y += layout->sub_elements[i]->rect.h;
			}
		}
		current_center.y = round((drawing_rect.h - current_center.y) / 2);
	}
	if (layout->arrangement == VENG_HORIZONTAL)
	{
		for (size_t i = 0; i < layout->sub_elements_size; i++)
		{
			if (layout->sub_elements[i] != NULL)
			{
				switch (layout->align_horizontal)
				{
					case VENG_LEFT:
						layout->sub_elements[i]->rect.x = drawing_rect.x + offset.x;
						break;
					case VENG_CENTER:
						layout->sub_elements[i]->rect.x = current_center.x;
						current_center.x += layout->sub_elements[i]->rect.w;
						break;
					case VENG_RIGHT:
						layout->sub_elements[i]->rect.x = drawing_rect.x + drawing_rect.w - layout->sub_elements[i]->rect.w - offset.x;
						break;
					default:
						printf("Invalid align_h argument.\n");
						return;
				}
				switch (layout->align_vertical)
				{
					case VENG_TOP:
						layout->sub_elements[i]->rect.y = drawing_rect.y;
						break;
					case VENG_CENTER:
						layout->sub_elements[i]->rect.y = drawing_rect.y + round((drawing_rect.h - layout->sub_elements[i]->rect.h)/2); 
						break;
					case VENG_BOTTOM:
						layout->sub_elements[i]->rect.y = drawing_rect.y + drawing_rect.h - layout->sub_elements[i]->rect.h;
						break;
					default:
						printf("Invalid align_v argument.\n");
						return;
				}
				offset.x += layout->sub_elements[i]->rect.w;
				// (III)
				VENG_PrepareChilds(layout->sub_elements[i], layout->sub_elements[i]->rect);
			}
		}
	}
	else if (layout->arrangement == VENG_VERTICAL)
	{
		for (size_t i = 0; i < layout->sub_elements_size; i++)
		{
			if (layout->sub_elements[i] != NULL)
			{
				switch (layout->align_horizontal)
				{
					case VENG_LEFT:
						layout->sub_elements[i]->rect.x = drawing_rect.x;
						break;
					case VENG_CENTER:
						layout->sub_elements[i]->rect.x = drawing_rect.x + round((drawing_rect.w - layout->sub_elements[i]->rect.w)/2);
						break;
					case VENG_RIGHT:
						layout->sub_elements[i]->rect.x = drawing_rect.x + drawing_rect.w - layout->sub_elements[i]->rect.w;
						break;
					default:
						printf("Invalid align_h argument.\n");
						return;
				}
				switch (layout->align_vertical)
				{
					case VENG_TOP:
						layout->sub_elements[i]->rect.y = drawing_rect.y + offset.y;
						break;
					case VENG_CENTER:
						layout->sub_elements[i]->rect.y = current_center.y;
						current_center.y += layout->sub_elements[i]->rect.h;
						break;
					case VENG_BOTTOM:
						layout->sub_elements[i]->rect.y = drawing_rect.y + drawing_rect.h - layout->sub_elements[i]->rect.h - offset.y;
						break;
					default:
						printf("Invalid align_v argument.\n");
						return;
				}
				offset.y += layout->sub_elements[i]->rect.h;
				// (III)
				VENG_PrepareChilds(layout->sub_elements[i], layout->sub_elements[i]->rect);
			}
		}
	}

	// (III)

	/*
	// Compute coords
	SDL_Point offset = (SDL_Point){0, 0};
	for (size_t i = 0; i < layout->sub_elements_size; i++)
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


	
	*/
}

VENG_Screen* VENG_GetScreen()
{
	return rendering_screen;
}

VENG_Driver VENG_GetDriver ()
{
	return driver;
}

SDL_Rect VENG_GetElementRect(VENG_Element* element)
{
	return element->rect;
}

SDL_Rect* VENG_GetElementRectPtr(VENG_Element* element)
{
	return &element->rect;
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
