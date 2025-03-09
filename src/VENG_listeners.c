#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "VENG.h"

#define LISTENERS_STARTING_HEAP 50
#define ENABLE_KEYBOARD_ALPHA false

// Pointer safety macro
#define IS_NULL(ptr) ((ptr) != NULL ? (ptr) : (printf("Pointer %s is NULL in line %d, %s.\n", #ptr, __LINE__, __FILE__), exit(1), NULL))

static VENG_MouseListener** mouse_listeners;
static size_t mouse_listeners_size;
static size_t mouse_listeners_capacity;

static char* keyboard_buffer;
static size_t keyboard_buffer_size;
static bool listening_keyboard;

static void check_mouse_listener(VENG_MouseListener* m_listener, SDL_Event* event);
static bool point_is_on_rect (Sint32 x, Sint32 y, SDL_Rect rect);

void VENG_Listen(SDL_Event event)
{
	if (event.type == SDL_TEXTINPUT && ENABLE_KEYBOARD_ALPHA)
	{
		strcat(keyboard_buffer, event.text.text);
	}
	for (size_t i = 0; i < mouse_listeners_size; i++)
	{
		if (mouse_listeners[i] != NULL)
		{	
			check_mouse_listener(mouse_listeners[i], &event);
		}
	}
}

static void check_mouse_listener(VENG_MouseListener* m_listener, SDL_Event* event)
{
	if (event->type == SDL_MOUSEMOTION && m_listener->m_trigger.m_motion)
	{
		point_is_on_rect(event->button.x, event->button.y, m_listener->element->rect) ? m_listener->on_call(m_listener->element, event) : NULL;
	}
	else if (event->type == SDL_MOUSEBUTTONDOWN && m_listener->m_trigger.m_button_down)
	{
		point_is_on_rect(event->button.x, event->button.y, m_listener->element->rect) ? m_listener->on_call(m_listener->element, event) : NULL;
	}
	else if (event->type == SDL_MOUSEBUTTONUP && m_listener->m_trigger.m_button_up)
	{
		point_is_on_rect(event->button.x, event->button.y, m_listener->element->rect) ? m_listener->on_call(m_listener->element, event) : NULL;
	}
	else if (event->type == SDL_MOUSEWHEEL && m_listener->m_trigger.m_wheel)
	{
		point_is_on_rect(event->wheel.mouseX, event->wheel.mouseY, m_listener->element->rect) ? m_listener->on_call(m_listener->element, event) : NULL;
	}
}

static bool point_is_on_rect (Sint32 x, Sint32 y, SDL_Rect rect)
{
	if (x < rect.x || x >= rect.x + rect.w)
	{
		return false;
	}
	if (y < rect.y || y >= rect.y + rect.h)
	{
		return false;
	}
	return true;
}

void VENG_AddMouseListener(VENG_Element* element, VENG_ListenerCallback on_call, VENG_MouseTrigger m_trigger)
{
	if (mouse_listeners == NULL)
	{
		mouse_listeners_size = LISTENERS_STARTING_HEAP;
		mouse_listeners = IS_NULL(calloc(mouse_listeners_size, sizeof(VENG_MouseListener)));
		mouse_listeners_capacity = 0;
	}
	if (mouse_listeners_capacity >= mouse_listeners_size)
	{
		mouse_listeners_size += LISTENERS_STARTING_HEAP;
		mouse_listeners = IS_NULL(realloc(mouse_listeners, mouse_listeners_size));
		for (size_t i = mouse_listeners_size - LISTENERS_STARTING_HEAP; i < mouse_listeners_size; i++)
		{
			mouse_listeners[i] = NULL;
		}
	}	

	for (size_t i = 0; i < mouse_listeners_size; i++)
	{
		if (mouse_listeners[i] == NULL)
		{
			mouse_listeners[i] = IS_NULL(calloc(1, sizeof(VENG_MouseListener)));	
			mouse_listeners[i]->element = element;
			mouse_listeners[i]->on_call = on_call;
			mouse_listeners[i]->m_trigger = m_trigger;
			mouse_listeners_capacity++;
			break;
		}
	}
}

VENG_MouseTrigger VENG_createMouseTrigger(bool m_motion, bool m_button_down, bool m_button_up, bool m_wheel)
{
	VENG_MouseTrigger m_trigger;
	m_trigger.m_motion = m_motion;
	m_trigger.m_button_down = m_button_down;
	m_trigger.m_button_up = m_button_up;
	m_trigger.m_wheel = m_wheel;
	return m_trigger;
}

void VENG_AttachKeyboardBuffer(char* buffer, size_t buffer_size)
{
	keyboard_buffer = buffer;
	keyboard_buffer_size = buffer_size;
	if (buffer == NULL)
	{
		keyboard_buffer_size = 0;
	}
}

void VENG_KeyboardStartListening()
{
	listening_keyboard = true;
	SDL_StartTextInput();
}

void VENG_KeyboardStopListening()
{
	listening_keyboard = false;
	SDL_StopTextInput();
}

bool VENG_KeyboardIsListening()
{
	return listening_keyboard;
}

void VENG_ResetListeners()
{
	mouse_listeners = NULL;
	mouse_listeners_size = 0;
	mouse_listeners_capacity = 0;

	keyboard_buffer = NULL;
	keyboard_buffer_size = 0;
	listening_keyboard = false;
}
