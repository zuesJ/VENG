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

#include <stdbool.h>

/*==========================================================================*\
 *                   VENG.c - Core Functions, structs & enums
\*==========================================================================*/

typedef enum VENG_ParentType VENG_ParentType;
typedef enum VENG_Arrangement VENG_Arrangement;
typedef enum VENG_Align VENG_Align;

typedef struct VENG_Driver VENG_Driver;
typedef struct VENG_Screen VENG_Screen;
typedef struct VENG_Element VENG_Element;
typedef struct VENG_Layout VENG_Layout;

typedef enum VENG_ParentType
{
	TYPE_SCREEN,
	TYPE_ELEMENT
} VENG_ParentType;

typedef enum VENG_Arrangement
{
	HORIZONTAL,
	VERTICAL
} VENG_Arrangement;

typedef enum VENG_Align
{
	LEFT,
	TOP,
	CENTER,
	RIGHT,
	BOTTOM
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
} VENG_Layout;

typedef struct VENG_Screen
{
	VENG_ParentType type;

	char* title;
	SDL_Surface* icon;
	
	VENG_Layout layout;
} VENG_Screen;

typedef struct VENG_Element
{
	VENG_ParentType type;

	SDL_Rect rect; // Px -> {x, y, w, h}
	float w, h;  // Width and height in % -> [0, 1]
	
	bool stretch_size;
	bool visible;
	
	VENG_Layout layout;
} VENG_Element;

/*
* Creates a layout
* Parameters: All the Layout's fields
*/
VENG_Layout VENG_CreateLayout(VENG_Arrangement arrangement, VENG_Align align_horizontal, VENG_Align align_vertical, VENG_Element** sub_elements, size_t sub_elements_size);

/*
* Creates a screen
* Parameters: All the screen's fields
*/
VENG_Screen VENG_CreateScreen(char* title, SDL_Surface* icon, VENG_Layout layout);

/*
* Creates an element
* Parameters: All the element's fields
*/
VENG_Element VENG_CreateElement(float w, float h, bool stretch_size, bool visible, VENG_Layout layout);

/*
* Creates a driver
* Paramters: A SDL window and renderer pointer
* Returns a VENG_Driver
*/
VENG_Driver VENG_CreateDriver(SDL_Window* window, SDL_Renderer* renderer);

/*
* Sets a driver as the main rendering scene
* Paramters: A VENG_Driver
*/
void VENG_SetDriver(VENG_Driver driver);

/*
* Sets a screen as the current rendering screen
* Parameters: a screen pointer
*/
void VENG_SetScreen(VENG_Screen* screen);

/*
* Fills the SDL_Rect field of the Element provided and also does the same for its sub-components. Created to be recursive.
* Parameters: a pointer to the Element, a void pointer to a parent Element* or Screen* and a SDL_Rect where the Element will be drawn,
*/
void VENG_PrepareElement(VENG_Element* element, void* parent_container, SDL_Rect drawing_rect);

/*
* Goes through all the Elements in the current screen and fills the SDL_Rect field of each Element according
* to the window size and custom sizes and positioning.
* Parameters: none
*/
void VENG_PrepareElements();

/*
* Starts the SDL subsystems
* Parameters: A driver struct
*/
void VENG_Init(VENG_Driver driver);

/*
* Shuts down the SDL subsystems
* Parameters: closeSDL: if this flag is true, it will also shutdown SDL and SDL_Image
*/
void VENG_Destroy(bool closeSDL);

/*
* Gets the current screen pointer
* Parameters: none
*/
VENG_Screen* VENG_GetScreen();

/*
* Gets the current driver
* Parameters: none
*/
VENG_Driver VENG_GetDriver();


/*==========================================================================*\
 *                    VENG_listeners.c - Input management
\*==========================================================================*/

typedef void (*VENG_ListenerCallback)(SDL_Event event);

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


/*==========================================================================*\
 *          VENG_tools.c - Useful functions for SDL & VENG development
\*==========================================================================*/

/*
* Loads a PNG into a SDL_Surface
* Parameters: the PNG's path
*/
SDL_Surface* VENG_LoadPNG (const char* path);


#endif