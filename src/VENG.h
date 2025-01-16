#ifndef VENG_H
#define VENG_H

/* ******************************************************
*
*			Structs
*
*			Enums
*
* *******************************************************/

// Forward declaration

typedef enum VENG_ParentType VENG_ParentType;
typedef enum VENG_Arrangement VENG_Arrangement;
typedef enum VENG_Align VENG_Align;

typedef struct VENG_Screen VENG_Screen;
typedef struct VENG_Element VENG_Element;
typedef struct VENG_Layout VENG_Layout;

typedef struct VENG_Driver
{
	SDL_Window* window;
	SDL_Renderer* renderer;
} VENG_Driver;

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

// Listeners


// VENG core functions
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
* Loads a PNG into a SDL_Surface
* Parameters: the PNG's path
*/
SDL_Surface* VENG_LoadPNG (const char* path);

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

#endif