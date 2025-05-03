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

#define ALLOCATED_LISTENER_START 1
static VENG_Listener** heap_listener = NULL;
static size_t listener_slots_size = ALLOCATED_LISTENER_START;
static size_t listener_slots_count = 0;

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
		printf("Error, VENG havent started\n");
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
		for (size_t i = listeners_slots_size - ALLOCATED_LISTENERS_START; i < listeners_slots_size; i++)
		{
			listeners[i] = NULL;
		}
	}
	for (size_t i = 0; i < layer->listeners->listeners_size; i++)
	{
		if (event->type == layer->listeners->listeners[i]->trigger)
		{
			if (layer->listeners->listeners[i] != NULL)
			{
				if (layer->listeners->listeners[i]->condition == NULL)
				{
					layer->listeners->listeners[i]->callback(layer->listeners->listeners[i]->element, event);
				}
				else if (layer->listeners->listeners[i]->condition(layer->listeners->listeners[i]->element, event) == 0)
				{
					layer->listeners->listeners[i]->callback(layer->listeners->listeners[i]->element, event);
				}
			}
		}
	}
	return 0;
}

int VENG_AddListenerToLayer(VENG_Listener* listener, VENG_Layer* layer)
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return -1;
	}
	else if (listener == NULL || layer == NULL)
	{
		printf("Layer or listener is NULL\n");
		return -1;		
	}
	if (layer->listeners == NULL)
	{
		if (listeners == NULL)
		{
			listeners = IS_NULL(calloc(listeners_slots_size, sizeof(VENG_Listeners*)));
		}
		else if (listeners_slots_count >= listeners_slots_size)
		{
			listeners_slots_size += ALLOCATED_LISTENERS_START;
			listeners = IS_NULL(realloc(listeners, listeners_slots_size * sizeof(VENG_Listeners*)));
			for (size_t i = listeners_slots_size - ALLOCATED_LISTENERS_START; i < listeners_slots_size; i++)
			{
				listeners[i] = NULL;
			}
		}
		for (size_t i = 0; i < listeners_slots_size; i++)
		{
			if (listeners[i] == NULL)
			{
				listeners[i] = (VENG_Listeners*)calloc(1, sizeof(VENG_Listeners));
				listeners[i]->listeners = (VENG_Listener**)calloc(1, sizeof(VENG_Listener*));
				listeners[i]->listeners_size = 1;
				listeners[i]->listeners_count = 0;
				layer->listeners = listeners[i];
				listeners_slots_count++;
				break;
			}
		}
	}
	else if (layer->listeners->listeners_count >= layer->listeners->listeners_size)
	{
		layer->listeners->listeners_size += ALLOCATED_LISTENER_START;
		layer->listeners->listeners = (VENG_Listener**)realloc(layer->listeners->listeners, layer->listeners->listeners_size * sizeof(VENG_Listener*));
		for (size_t i = layer->listeners->listeners_size - ALLOCATED_LISTENER_START; i < layer->listeners->listeners_size; i++)
		{
			layer->listeners->listeners[i] = NULL;
		}
	}
	for (size_t i = 0; i < layer->listeners->listeners_size; i++)
	{
		if (layer->listeners->listeners[i] == NULL)
		{
			layer->listeners->listeners[i] = listener;
			layer->listeners->listeners_count++;
			break;
		}
	}

	return 0;
}

VENG_Listener* VENG_CreateListener(SDL_EventType trigger, VENG_ListenerCallback callback, VENG_ListenerCondition condition, VENG_Element* element)
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return NULL;
	}
	else if (element == NULL)
	{
		printf("element cant be null\n");
	}
	else if (heap_listener == NULL)
	{
		heap_listener = (VENG_Listener**)calloc(listener_slots_count, sizeof(VENG_Listener*));
	}
	else if (listener_slots_count >= listener_slots_size)
	{
		listener_slots_size += ALLOCATED_LISTENER_START;
		heap_listener = (VENG_Listener**)realloc(heap_listener, listener_slots_size * sizeof(VENG_Listener*));
		for (size_t i = listener_slots_size - ALLOCATED_LISTENER_START; i < listener_slots_size; i++)
		{
			heap_listener[i] = NULL;
		}

	}
	VENG_Listener* to_return;
	for (size_t i = 0; i < listener_slots_size; i++)
	{
		if (heap_listener[i] == NULL)
		{
			heap_listener[i] = (VENG_Listener*)calloc(1, sizeof(VENG_Listener));
			heap_listener[i]->trigger = trigger;
			heap_listener[i]->callback = callback;
			heap_listener[i]->condition = condition;
			heap_listener[i]->element = element;
			to_return = heap_listener[i];
			listener_slots_count++;
			break;
		}
	}
	return to_return;
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
	if (listeners != NULL)
	{
		for (size_t i = 0; i < listeners_slots_size; i++)
		{
			printf("\tSlot %ld: %p\n", i, listeners[i]);
		}
	}
	printf("VENG listener_heap: %p ; lh_size:%ld ; lh_count: %ld\n", heap_listener, listener_slots_size, listener_slots_count);
	if (heap_listener == NULL) return 0;
	for (size_t i = 0; i < listener_slots_size; i++)
	{
		printf("\tSlot %ld: %p\n", i, heap_listener[i]);
	}
	return 0;
}

int VENG_PrintLayerListeners(VENG_Layer* layer)
{
	if (!VENG_HasStarted())
	{
		printf("Error, veng havent started\n");
		return -1;
	}
	else if (layer == NULL)
	{
		printf("null ptr layer\n");
		return -1;
	}
	else if (layer->listeners == NULL)
	{
		printf("Layer: %p ; Layer_Listeners: %p\n", layer, layer->listeners);
		return 0;
	}
	printf("Layer: %p ; Layer_Listeners: %p\n", layer, layer->listeners);
	if (layer->listeners->listeners == NULL)
	{
		printf("\tptr: %p ; size: %ld ; count: %ld\n", layer->listeners->listeners, layer->listeners->listeners_size, layer->listeners->listeners_count);
		return 0;
	}
	printf("\tptr: %p ; size: %ld ; count: %ld\n", layer->listeners->listeners, layer->listeners->listeners_size, layer->listeners->listeners_count);
	for (size_t i = 0; i < layer->listeners->listeners_size; i++)
	{
		printf("\t\tSlot %ld: %p\n", i, layer->listeners->listeners[i]);
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
