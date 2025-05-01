#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "VENG.h"

// Pointer safety
static void* IS_NULL(void *ptr);

#define ALLOCATED_LISTENERS_START 1
static VENG_Listeners** listeners = NULL;
static size_t listeners_slots_size = ALLOCATED_LISTENERS_START;
static size_t listeners_slots_count = 0;

int VENG_ListenScreen(SDL_Event* event, VENG_Screen* screen)
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return 1;
	}
	else if (event == NULL || screen == NULL)
	{
		printf("Error, NULL pointer in event or screen\n");
		return 1;
	}
	else if (screen->layers == NULL || screen->layers_size == 0 || screen->layers_count == 0)
	{
		printf("Warning: The screen given doesnt provide any layer\n");
		return 0;
	}
	else if (listeners == NULL)
	{
		listeners = IS_NULL(calloc(listeners_slots_size, sizeof(VENG_Listeners*)));
	}
	else if (listeners_slots_count >= listeners_slots_size)
	{
		listeners_slots_size += ALLOCATED_LISTENERS_START;
		listeners = IS_NULL(realloc(listeners, listeners_slots_size * sizeof(VENG_Listeners*)));
	}
	for (size_t i = 0; i < screen->layers_size; i++)
	{
		if (screen->layers[i] != NULL)
		{
			if (screen->layers[i]->listeners != NULL) VENG_ListenLayer(event, screen->layers[i]);
		}
	}
	return 0;
}

int VENG_ListenLayer(SDL_Event* event, VENG_Layer* layer)
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return 1;
	}
	else if (event == NULL || layer == NULL)
	{
		printf("Error, NULL pointer in event or layer\n");
		return 1;
	}
	else if (layer->listeners == NULL)
	{
		printf("Error: the layer doesnt have listeners\n");
		return 1;
	}
	else if (layer->listeners->listeners == NULL || layer->listeners->listeners_size == 0 || layer->listeners->listeners_count == 0)
	{
		printf("Error: the layer doesnt have listeners\n");
		return 1;
	}
	else if (listeners == NULL)
	{
		listeners = IS_NULL(calloc(listeners_slots_size, sizeof(VENG_Listeners*)));
	}
	else if (listeners_slots_count >= listeners_slots_size)
	{
		listeners_slots_size += ALLOCATED_LISTENERS_START;
		listeners = IS_NULL(realloc(listeners, listeners_slots_size * sizeof(VENG_Listeners*)));
	}
	for (size_t i = 0; i < layer->listeners->listeners_size; i++)
	{
		if (layer->listeners->listeners[i] != NULL)
		{
			if (layer->listeners->listeners[i].condition == NULL)
			{
				layer->listeners->listeners[i].callback(layer->listeners->listeners[i].element, event);
			}
			else if (layer->listeners->listeners[i].condition(layer->listeners->listeners[i].element, event) == 0)
			{
				layer->listeners->listeners[i].callback(layer->listeners->listeners[i].element, event);
			}
		}
	}
	return 0;
}

int VENG_AddListenerToLayer(VENG_Listener listener, VENG_Layer* layer)
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return 1;
	}
	else if (layer == NULL)
	{
		printf("Layer is NULL\n");
		return -1;		
	}
	
	return 0;
}

VENG_Listener* VENG_CreateListener(SDL_EventType trigger, VENG_ListenerCallback callback, VENG_ListenerCondition condition, VENG_Element* element)
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return (VENG_Listener){0};
	}
	else if (element == NULL)
	{
		printf("element cant be null\n");
	}
	VENG_Listener listener;
	listener.trigger = trigger;
	listener.callback = callback;
	listener.condition = condition;
	listener.element = element;
	return &listener;
}

// Debug
int VENG_PrintListenersInternalHierarchy()
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return 1;
	}
	printf("VENG Listeners: %p ; size : %ld ; count : %ld\n", listeners, listeners_slots_size, listeners_slots_count);
	if (listeners == NULL) return 0;
	for (size_t i = 0; i < listeners_slots_size; i++)
	{
		printf("\tSlot %ld: %p\n", i, listeners[i]);
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
