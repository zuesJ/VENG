#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Pointer safety
static void* IS_NULL(void *ptr);

// SDL2 lib
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "VENG/VENG.h"

static bool started = false;

static VENG_Driver driver;
static VENG_Screen* rendering_screen;

#define ALLOCATED_SCREENS_START 1
static VENG_Screen** screens = NULL;
static size_t screen_slots_size = ALLOCATED_SCREENS_START;
static size_t screen_slots_count = 0;

#define ALLOCATED_LAYERS_START 1
static VENG_Layer** layers = NULL;
static size_t layer_slots_size = ALLOCATED_LAYERS_START;
static size_t layer_slots_count = 0;

#define ALLOCATED_ELEMENTS_START 1
static VENG_Element** elements = NULL;
static size_t elements_slots_size = ALLOCATED_ELEMENTS_START;
static size_t elements_slots_count = 0;

/*==========================================================================*\
 *                   			Start and finish
\*==========================================================================*/
int VENG_Init(VENG_Driver new_driver)
{
	if (started)
	{
		printf("VENG has already started\n");
		return 1;
	}
	else if (new_driver.window == NULL || new_driver.renderer == NULL)
	{
		printf("Driver contains NULL contents\n");
		return 1;
	}
	started = true;
	VENG_SetDriver(new_driver);
	
	// Start heap
	screens = IS_NULL(calloc(ALLOCATED_SCREENS_START, sizeof(VENG_Screen*)));
	layers = IS_NULL(calloc(ALLOCATED_LAYERS_START, sizeof(VENG_Layer*)));
	elements = IS_NULL(calloc(ALLOCATED_ELEMENTS_START, sizeof(VENG_Element*)));

	return 0;
}

void VENG_Destroy()
{
	if (!VENG_HasStarted()) return;
	driver = (VENG_Driver){NULL, NULL};
	rendering_screen = NULL;
	started = false;
}

/*==========================================================================*\
 *                   				Creators
\*==========================================================================*/
VENG_Screen* VENG_CreateScreen(char* title, SDL_Surface* icon, size_t max_layers)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return NULL;
	}
	else if (title == NULL)
	{
		printf("title pointer is NULL\n");
		return NULL;
	}
	else if (max_layers == 0)
	{
		printf("Max layers cannot be 0, the minimum is 1\n");
		return NULL;
	}

	if (screen_slots_count >= screen_slots_size)
	{
		screen_slots_size += ALLOCATED_SCREENS_START;
		screens = (VENG_Screen**)IS_NULL(realloc(screens, screen_slots_size * sizeof(VENG_Screen*)));
		// As realloc does not clear the new heap, it needs to be manually cleared
		for (size_t i = screen_slots_size - ALLOCATED_SCREENS_START; i < screen_slots_size; i++)
		{
			screens[i] = NULL;
		}
	}

	VENG_Screen* return_adress;
	for (size_t i = 0; i < screen_slots_size; i++)
	{
		if (screens[i] == NULL)
		{
			screens[i] = IS_NULL(calloc(1, sizeof(VENG_Screen)));
			screens[i]->type = VENG_TYPE_SCREEN;
			screens[i]->title = title;
			screens[i]->icon = icon;
			screens[i]->layers = IS_NULL(calloc(max_layers, sizeof(VENG_Layer*)));
			screens[i]->layers_size = max_layers;
			screens[i]->layers_count = 0;
			return_adress = screens[i];
			break;
		}
	}
	screen_slots_count++;
	return return_adress;
}

VENG_Layer* VENG_CreateLayer(VENG_Layout layout, size_t max_elements)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return NULL;
	}

	if (max_elements == 0)
	{
		printf("Max elements cannot be 0, the minimum is 1\n");
		return NULL;
	}

	if (layer_slots_count >= layer_slots_size)
	{
		layer_slots_size += ALLOCATED_LAYERS_START;
		layers = (VENG_Layer**)IS_NULL(realloc(layers, layer_slots_size * sizeof(VENG_Layer*)));
		// As realloc does not clear the new heap, it needs to be manually cleared
		for (size_t i = layer_slots_size - ALLOCATED_LAYERS_START; i < layer_slots_size; i++)
		{
			layers[i] = NULL;
		}
	}

	VENG_Layer* return_adress;
	for (size_t i = 0; i < layer_slots_size; i++)
	{
		if (layers[i] == NULL)
		{
			layers[i] = (VENG_Layer*)IS_NULL(calloc(1, sizeof(VENG_Layer)));
			layers[i]->type = VENG_TYPE_LAYER;
			layers[i]->layout = layout;
			layers[i]->childs.sub_elements = IS_NULL(calloc(max_elements, sizeof(VENG_Element*)));
			layers[i]->childs.sub_elements_size = max_elements;
			layers[i]->childs.sub_elements_count = 0;
			return_adress = layers[i];
			break;
		}
	}
	layer_slots_count++;
	return return_adress;
}

VENG_Element* VENG_CreateElement(float w, float h, bool stretch_size, bool visible, VENG_Layout layout, size_t max_sub_elements)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return NULL;
	}
	else if (w < 0 || h < 0)
	{
		printf("W and H cannot be negative\n");
	}
	else if (elements_slots_count >= elements_slots_size)
	{
		elements_slots_size += ALLOCATED_ELEMENTS_START;
		elements = (VENG_Element**)IS_NULL(realloc(elements, elements_slots_size * sizeof(VENG_Element*)));
		for (size_t i = elements_slots_size - ALLOCATED_ELEMENTS_START; i < elements_slots_size; i++)
		{
			elements[i] = NULL;
		}
	}

	VENG_Element* return_adress;
	for (size_t i = 0; i < elements_slots_size; i++)
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
			elements[i]->childs.sub_elements_size = max_sub_elements;
			if (max_sub_elements == 0)
			{
				elements[i]->childs.sub_elements = NULL;	
			}
			else
			{
				elements[i]->childs.sub_elements = IS_NULL(calloc(max_sub_elements, sizeof(VENG_Element*)));
			}
			elements[i]->childs.sub_elements_count = 0;
			elements[i]->dirty = true;
			return_adress = elements[i];
			break;
		}
	}
	elements_slots_count++;
	return return_adress;
}

VENG_Layout VENG_CreateLayout(VENG_Arrangement arrangement, VENG_Align align_horizontal, VENG_Align align_vertical)
{
	VENG_Layout layout;
	layout.arrangement = arrangement;
	layout.align_horizontal = align_horizontal;
	layout.align_vertical = align_vertical;

	return layout;
}

VENG_Driver VENG_CreateDriver(SDL_Window* window, SDL_Renderer* renderer)
{
	if (window == NULL || renderer == NULL)
	{
		printf("Window or renderer cannot be NULL\n");
		return (VENG_Driver){NULL, NULL};
	}
	VENG_Driver driver;
	driver.window = window;
	driver.renderer = renderer;
	return driver;
}

/*==========================================================================*\
 *                   				Add
\*==========================================================================*/
int VENG_AddLayerToScreen(VENG_Layer* layer, VENG_Screen* screen)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return;
	}
	if (layer == NULL || screen == NULL)
	{
		printf("Layer or screen cannot be NULL\n");
		return 1;
	}
	if (screen->layers == NULL)
	{
		screen->layers = IS_NULL(calloc(screen->layers_size, sizeof(VENG_Layer*)));
		screen->layers_count = 0;
	}
	if (screen->layers_count >= screen->layers_size)
	{
		printf("Couldn't add Layer: max size reached\n");
		return 1;
	}
	for (size_t i = 0; i < screen->layers_size; i++)
	{
		if (screen->layers[i] == NULL)
		{
			screen->layers[i] = layer;
			screen->layers_count += 1;
			break;
		}
	}
	return 0;
}

int VENG_AddElementToLayer(VENG_Element* element, VENG_Layer* layer)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return 1:
	}
	if (element == NULL || layer == NULL)
	{
		printf("Element or layer cannot be NULL\n");
		return 1;
	}
	if (layer->childs.sub_elements == NULL)
	{
		layer->childs.sub_elements = IS_NULL(calloc(layer->childs.sub_elements_size, sizeof(VENG_Element*)));
		layer->childs.sub_elements_count = 0;
	}
	if (layer->childs.sub_elements_count >= layer->childs.sub_elements_size)
	{
		printf("Couldn't add Element: max size reached\n");
		return 1;
	}
	for (size_t i = 0; i < layer->childs.sub_elements_size; i++)
	{
		if (layer->childs.sub_elements[i] == NULL)
		{
			layer->childs.sub_elements[i] = element;
			layer->childs.sub_elements_count += 1;
			break;
		}
	}
	return 0;
}

int VENG_AddSubElementToElement(VENG_Element* sub_element, VENG_Element* element)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return 1;
	}
	else if (sub_element == NULL || element == NULL)
	{
		printf("Sub_element or Element cannot be NULL\n");
		return 1;
	}
	if (element->childs.sub_elements == NULL)
	{
		element->childs.sub_elements = IS_NULL(calloc(element->childs.sub_elements_size, sizeof(VENG_Element*)));
		element->childs.sub_elements_count = 0;
	}
	if (element->childs.sub_elements_count >= element->childs.sub_elements_size)
	{
		printf("Couldn't add Element: max size reached\n");
		return 1;
	}
	for (size_t i = 0; i < element->childs.sub_elements_size; i++)
	{
		if (element->childs.sub_elements[i] == NULL)
		{
			element->childs.sub_elements[i] = sub_element;
			element->childs.sub_elements_count += 1;
			break;
		}
	}
	return 0;
}

/*==========================================================================*\
 *                   				 Set
\*==========================================================================*/
int VENG_SetScreen(VENG_Screen* screen)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return 1;
	}
	if (screen == NULL)
	{
		SDL_SetWindowTitle(driver.window, "VENG");
		SDL_SetWindowIcon(driver.window, NULL);
		return 0;
	}
	rendering_screen = screen;
	if (screen->icon != NULL)
	{
		SDL_SetWindowIcon(driver.window, screen->icon);
	}
	SDL_SetWindowTitle(driver.window, screen->title);
	return 0;
}

int VENG_SetDriver(VENG_Driver new_driver)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return 1;
	}
	if (new_driver.window == NULL || new_driver.renderer == NULL)
	{
		return 1;
	}
	driver = new_driver;
	return 0;
}

/*==========================================================================*\
 *                   				 Get
\*==========================================================================*/
VENG_Screen* VENG_GetScreen()
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return NULL;
	}
	return rendering_screen;
}

VENG_Driver VENG_GetDriver()
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return (VENG_Driver){0};
	}
	return driver;
}

SDL_Rect VENG_GetElementRect(VENG_Element* element)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return (SDL_Rect){0};
	}
	else if (element == NULL)
	{
		printf("Element is NULL\n");
		return NULL;
	}
	return element->rect;
}

SDL_Rect* VENG_GetElementRectPtr(VENG_Element* element)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return NULL;
	}
	else if (element == NULL)
	{
		printf("Element is NULL\n");
		return NULL;
	}
	return &element->rect;
}


/*==========================================================================*\
 *                   			   Prepare
\*==========================================================================*/
int VENG_PrepareScreen(VENG_Screen* screen)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return 1;
	}
	if (screen == NULL)
	{
		printf("Screen is NULL\n");
		return 1;
	}
	if (screen->layers == NULL || screen->layers_count == 0)
	{
		return 0;
	}
	for (size_t i = 0; i < screen->layers_size; i++)
	{
		if (screen->layers[i] != NULL)
		{
			VENG_PrepareLayer(screen->layers[i]);
		}
	}
	return 0;
}

int VENG_PrepareLayer(VENG_Layer* layer)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return 1;
	}
	if (layer == NULL)
	{
		printf("Layer is NULL");
		return 1;
	}
	int window_w, window_h;
	SDL_GetRendererOutputSize(driver.renderer, &window_w, &window_h);
	VENG_PrepareElements(layer, (SDL_Rect){0, 0, window_w, window_h});
	return 0;
}

void VENG_PrepareElements(void* parent_container, SDL_Rect drawing_rect)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return;
	}
	// (I) Compute every child's size
	// (II) Once computed, align every child
	// (III) Check if the childs have more childs
	/*This is weird, isn't it?*/ 
	
	// void* parent_container -----> layout & childs

	if (parent_container == NULL)
	{
		return;
	}

	VENG_Layout* layout = NULL;
	VENG_Childs* childs = NULL;
	{
		VENG_Layer* data = (VENG_Layer*)parent_container;
		VENG_ParentType type = data->type;
		if (type == VENG_TYPE_LAYER)
		{
			layout = &data->layout;
			childs = &data->childs;
		}
		else if (type == VENG_TYPE_ELEMENT)
		{
			VENG_Element* element = (VENG_Element*)parent_container;
			layout = &element->layout;
			childs = &element->childs;
		}
		else
		{
			printf("Invalid Parent_Container\n");
			return;
		}
	}

	if (layout == NULL || childs == NULL)
	{
		return;
	}

	if (childs->sub_elements == NULL || childs->sub_elements_size == 0 || childs->sub_elements_count == 0)
	{
		return;
	}

	// (I)
	float screen_ratio = (float)drawing_rect.w / drawing_rect.h;
	for (size_t i = 0; i < childs->sub_elements_size; i++)
	{
		if (childs->sub_elements[i] != NULL)
		{
			if (!childs->sub_elements[i]->visible)
			{
				childs->sub_elements[i]->rect = (SDL_Rect){-1, -1, -1, -1};
				continue;
			}
			if (childs->sub_elements[i]->stretch_size)
			{
				childs->sub_elements[i]->rect.w = round(childs->sub_elements[i]->w * drawing_rect.w);
				childs->sub_elements[i]->rect.h = round(childs->sub_elements[i]->h * drawing_rect.h);
			}
			else
			{
				if (screen_ratio >= 1.0f)	
				{
					childs->sub_elements[i]->rect.w = round(childs->sub_elements[i]->w * drawing_rect.h);
					childs->sub_elements[i]->rect.h = round(childs->sub_elements[i]->h * drawing_rect.h);
				}
				else
				{
					childs->sub_elements[i]->rect.w = round(childs->sub_elements[i]->w * drawing_rect.w);
					childs->sub_elements[i]->rect.h = round(childs->sub_elements[i]->h * drawing_rect.w);
				}
			}
		}
	} 

	// (II)
	SDL_Point offset = (SDL_Point){0, 0};
	SDL_Point current_center = (SDL_Point){0, 0};
	if (layout->align_horizontal == VENG_CENTER)
	{
		for (size_t i = 0; i < childs->sub_elements_size; i++)
		{
			if (childs->sub_elements[i] != NULL)
			{
				current_center.x += childs->sub_elements[i]->rect.w;
			}
		}
		current_center.x = round((drawing_rect.w - current_center.x) / 2);
	}
	if (layout->align_vertical == VENG_CENTER)
	{
		for (size_t i = 0; i < childs->sub_elements_size; i++)
		{
			if (childs->sub_elements[i] != NULL)
			{
				current_center.y += childs->sub_elements[i]->rect.h;
			}
		}
		current_center.y = round((drawing_rect.h - current_center.y) / 2);
	}
	if (layout->arrangement == VENG_HORIZONTAL)
	{
		for (size_t i = 0; i < childs->sub_elements_size; i++)
		{
			if (childs->sub_elements[i] != NULL)
			{
				switch (layout->align_horizontal)
				{
					case VENG_LEFT:
						childs->sub_elements[i]->rect.x = drawing_rect.x + offset.x;
						break;
					case VENG_CENTER:
						childs->sub_elements[i]->rect.x = drawing_rect.x + current_center.x;
						current_center.x += childs->sub_elements[i]->rect.w;
						break;
					case VENG_RIGHT:
						childs->sub_elements[i]->rect.x = drawing_rect.x + drawing_rect.w - childs->sub_elements[i]->rect.w - offset.x;
						break;
					default:
						printf("Invalid align_h argument.\n");
						return;
				}
				switch (layout->align_vertical)
				{
					case VENG_TOP:
						childs->sub_elements[i]->rect.y = drawing_rect.y;
						break;
					case VENG_CENTER:
						childs->sub_elements[i]->rect.y = drawing_rect.y + round((drawing_rect.h - childs->sub_elements[i]->rect.h)/2); 
						break;
					case VENG_BOTTOM:
						childs->sub_elements[i]->rect.y = drawing_rect.y + drawing_rect.h - childs->sub_elements[i]->rect.h;
						break;
					default:
						printf("Invalid align_v argument.\n");
						return;
				}
				offset.x += childs->sub_elements[i]->rect.w;
				// (III)
				VENG_PrepareElements(childs->sub_elements[i], childs->sub_elements[i]->rect);
			}
		}
	}
	else if (layout->arrangement == VENG_VERTICAL)
	{
		for (size_t i = 0; i < childs->sub_elements_size; i++)
		{
			if (childs->sub_elements[i] != NULL)
			{
				switch (layout->align_horizontal)
				{
					case VENG_LEFT:
						childs->sub_elements[i]->rect.x = drawing_rect.x;
						break;
					case VENG_CENTER:
						childs->sub_elements[i]->rect.x = drawing_rect.x + round((drawing_rect.w - childs->sub_elements[i]->rect.w)/2);
						break;
					case VENG_RIGHT:
						childs->sub_elements[i]->rect.x = drawing_rect.x + drawing_rect.w - childs->sub_elements[i]->rect.w;
						break;
					default:
						printf("Invalid align_h argument.\n");
						return;
				}
				switch (layout->align_vertical)
				{
					case VENG_TOP:
						childs->sub_elements[i]->rect.y = drawing_rect.y + offset.y;
						break;
					case VENG_CENTER:
						childs->sub_elements[i]->rect.y = drawing_rect.y + current_center.y;
						current_center.y += childs->sub_elements[i]->rect.h;
						break;
					case VENG_BOTTOM:
						childs->sub_elements[i]->rect.y = drawing_rect.y + drawing_rect.h - childs->sub_elements[i]->rect.h - offset.y;
						break;
					default:
						printf("Invalid align_v argument.\n");
						return;
				}
				offset.y += childs->sub_elements[i]->rect.h;
				// (III)
				VENG_PrepareElements(childs->sub_elements[i], childs->sub_elements[i]->rect);
			}
		}
	}
}

/*==========================================================================*\
 *                   				Drawing
\*==========================================================================*/
SDL_Rect VENG_StartDrawing(VENG_Element* element)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return (SDL_Rect){0};
	}
	if (element == NULL)
	{
		printf("Element is NULL\n");
		return (SDL_Rect){-1, -1, -1, -1};
	}
	SDL_RenderSetClipRect(driver.renderer, &element->rect);
	return element->rect;
}

void VENG_StopDrawing(SDL_Rect* target)
{
	if (!VENG_HasStarted())
	{
		printf("VENG is not initialized yet\n");
		return;
	}
	SDL_RenderSetClipRect(driver.renderer, target);
}

/*==========================================================================*\
 *                   			 Optimization
\*==========================================================================*/
// An Element should be flagged as dirty if:
//	- The screen dimentions got modified
//	- The element 
//	-

/*==========================================================================*\
 *                   				Debug
\*==========================================================================*/
bool VENG_HasStarted()
{
	return started;
}

void VENG_PrintInternalHierarchy()
{
	printf("VENG: started:%db ; Driver: {w:%p r:%p} ; RenderingScreen: %p\n", started, driver.window, driver.renderer, rendering_screen);
	printf("Screens: used: %ld ; total: %ld\n", screen_slots_count, screen_slots_size);
	for (size_t i = 0; i < screen_slots_size; i++)
	{
		printf("\tSlot %ld: %p\n", i, screens[i]);
	}
	printf("Layers: used: %ld ; total: %ld\n", layer_slots_count, layer_slots_size);
	for (size_t i = 0; i < layer_slots_size; i++)
	{
		printf("\tSlot %ld: %p\n", i, layers[i]);
	}
	printf("Elements: used: %ld ; total: %ld\n", elements_slots_count, elements_slots_size);
	for (size_t i = 0; i < elements_slots_size; i++)
	{
		printf("\tSlot %ld: %p\n", i, elements[i]);
	}
}

static int __PrintElementHierarchy (VENG_Element* element, size_t tabs);
int VENG_PrintScreenHierarchy(VENG_Screen* screen)
{
	if (screen)
	printf("Screen: %s ; IconPtr: %p ; l_ptr: %p ; l_size: %zd ; l_count: %zd ; type: %d\n", screen->title, screen->icon, screen->layers, screen->layers_size, screen->layers_count, screen->type);
	for (size_t i = 0; i < screen->layers_size; i++)
	{
		if (screen->layers[i] != NULL)
		{
			printf("\tLayer %ld: %p ; layout: {a:%d, ah:%d av:%d} ; e_ptr: %p; e_size: %ld ; e_count: %ld ; type: %d\n",
				 	i, screen->layers[i], screen->layers[i]->layout.arrangement, screen->layers[i]->layout.align_horizontal,
					screen->layers[i]->layout.align_vertical, screen->layers[i]->childs.sub_elements,
					screen->layers[i]->childs.sub_elements_size, screen->layers[i]->childs.sub_elements_count, screen->layers[i]->type);
			for (size_t k = 0; k < screen->layers[i]->childs.sub_elements_size; k++)
			{
				printf("\t\tElement %ld: ", k);
				__PrintElementHierarchy(screen->layers[i]->childs.sub_elements[k], 3);
			}
		}
		else 
		{
			printf("\tLayer %ld: %p\n", i, screen->layers[i]);
		}
	}
	return 0;
}

int __PrintElementHierarchy (VENG_Element* element, size_t tabs)
{
	if (element == NULL)
	{
		printf("%p\n", element);
		return 1;
	}
	else
	{
		printf("%p, rect: {%d %d %d %d} ; dim: {%f %f} ; str_size:%db ; visible:%db ; dirty:%db ; layout: {a:%d, ah:%d av:%d} ; c_ptr: %p ; c_size: %ld ; c_count: %ld\n", element,
				element->rect.x, element->rect.y, element->rect.w, element->rect.h, element->w, element->h, element->stretch_size, element->visible,
				element->dirty, element->layout.arrangement, element->layout.align_horizontal, element->layout.align_vertical, element->childs.sub_elements,
				element->childs.sub_elements_size, element->childs.sub_elements_count);
		if (element->childs.sub_elements != NULL)
		{
			for (size_t i = 0; i < element->childs.sub_elements_size; i++)
			{
				for (size_t tab = 0; tab < tabs; tab++)
				{
					printf("\t");
				}
				if (element->childs.sub_elements[i] != NULL)
				{
					printf("Element %ld: ", i);
					__PrintElementHierarchy(element->childs.sub_elements[i], ++tabs);
				}
				else
				{
					printf("Element %ld: %p\n", i, element->childs.sub_elements[i]);
				}
			}
		}
	}
	return 0;
}

static void* IS_NULL(void *ptr) 
{
    if (!ptr) 
    {
        printf("Pointer %p is NULL\n", ptr);
        exit(EXIT_FAILURE);
    }
    return ptr;

}




