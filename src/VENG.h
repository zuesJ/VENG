/*==========================================================================*\
 *                           VENG.h - VENG Library Header
 * ===========================================================================
 * This header contains all the function declarations and type definitions
 * required for the VENG library. It acts as the primary interface to access
 * GUI management functionality within SDL.
 *
 * MIT License:
 * Copyright (c) 2025 Daniel García
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
\*==========================================================================*/

#ifndef VENG_H
#define VENG_H

#include <SDL2/SDL.h>
#include <stdbool.h>

/*==========================================================================*\
 *                   VENG.c - Core Functions, structs & enums
\*==========================================================================*/

typedef enum VENG_ParentType VENG_ParentType;

typedef struct VENG_Element VENG_Element;
typedef struct VENG_Layout VENG_Layout;

typedef enum VENG_Arrangement VENG_Arrangement;
typedef enum VENG_Align VENG_Align;

typedef struct VENG_Screen VENG_Screen;
typedef struct VENG_Driver VENG_Driver;

typedef enum VENG_ParentType
{
	VENG_TYPE_SCREEN,
	VENG_TYPE_ELEMENT
} VENG_ParentType;

typedef enum VENG_Arrangement
{
	VENG_HORIZONTAL,
	VENG_VERTICAL
} VENG_Arrangement;

typedef enum VENG_Align
{
	VENG_LEFT,
	VENG_TOP,
	VENG_CENTER,
	VENG_RIGHT,
	VENG_BOTTOM
} VENG_Align;

typedef struct VENG_Driver
{
	SDL_Window* window;
	SDL_Renderer* renderer;
} VENG_Driver;

typedef struct VENG_Layout
{
	VENG_Arrangement arrangement;
	VENG_Align align_horizontal;
	VENG_Align align_vertical;

	VENG_Element** sub_elements;
	size_t sub_elements_size;
	size_t sub_elements_count;
} VENG_Layout;

typedef struct VENG_Screen
{
	VENG_ParentType type;

	char* title;
	SDL_Surface* icon;
	
	VENG_Layout layout;

	bool dirty;
} VENG_Screen;

typedef struct VENG_Element
{
	VENG_ParentType type;

	SDL_Rect rect; // Px -> {x, y, w, h}
	float w, h;    // Width and height in % -> [0, 1]
	
	bool stretch_size;
	bool visible;

	bool dirty;

	VENG_Layout layout;
} VENG_Element;

// Start and finish
int VENG_Init(VENG_Driver driver);

void VENG_Destroy(bool closeSDL);

// Create
VENG_Driver VENG_CreateDriver(SDL_Window* window, SDL_Renderer* renderer);

VENG_Screen* VENG_CreateScreen(char* title, SDL_Surface* icon, VENG_Layout layout);

VENG_Element* VENG_CreateElement(float w, float h, bool stretch_size, bool visible, VENG_Layout layout);

VENG_Layout VENG_CreateLayout(VENG_Arrangement arrangement, VENG_Align align_horizontal, VENG_Align align_vertical, size_t max_childs);

// Debug
void VENG_PrintScreenHierarchy(VENG_Screen* screen);

// Add 
void VENG_AddElementToScreen(VENG_Element* element, VENG_Screen* screen);

void VENG_AddSubElementToElement(VENG_Element* sub_element, VENG_Element* element);

// Prepare
void VENG_PrepareScreen(VENG_Screen* screen);

void VENG_PrepareChilds(void* parent_container, SDL_Rect drawing_rect);

// Set
void VENG_SetDriver(VENG_Driver driver);

void VENG_SetScreen(VENG_Screen* screen);

// Drawing
SDL_Rect VENG_StartDrawing(VENG_Element* element);

void VENG_StopDrawing(SDL_Rect* viewport);

// Get
SDL_Rect VENG_GetElementRect(VENG_Element* element);

SDL_Rect* VENG_GetElementRectPtr(VENG_Element* element);

VENG_Screen* VENG_GetScreen();

VENG_Driver VENG_GetDriver();

/*==========================================================================*\
 *                    VENG_listeners.c - Input management 
\*==========================================================================*/

typedef void (*VENG_ListenerCallback)(VENG_Element* element, SDL_Event* event);

typedef struct VENG_MouseListener VENG_MouseListener;
typedef struct VENG_MouseTrigger VENG_MouseTrigger;

typedef struct VENG_MouseTrigger
{
	bool m_motion;
	bool m_button_down;
	bool m_button_up;
	bool m_wheel;
} VENG_MouseTrigger;

typedef struct VENG_MouseListener
{
	VENG_Element* element;
	VENG_ListenerCallback on_call;
	VENG_MouseTrigger m_trigger;
} VENG_MouseListener;

void VENG_Listen(SDL_Event event);

VENG_MouseTrigger VENG_createMouseTrigger(bool m_motion, bool m_button_down, bool m_button_up, bool m_wheel);

void VENG_AddMouseListener(VENG_Element* element, VENG_ListenerCallback on_call, VENG_MouseTrigger m_trigger);

// Note: Keyboard detection is in a young phase. There are bugs.

void VENG_AttachKeyboardBuffer(char* buffer, size_t buffer_size);

void VENG_KeyboardStartListening();

void VENG_KeyboardStopListening();

bool VENG_KeyboardIsListening();

void VENG_ResetListeners();

#endif