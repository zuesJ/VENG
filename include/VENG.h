#ifndef VENG_H
#define VENG_H

// Structs - Enums
typedef enum VENG_ParentType
{
	TYPE_SCREEN,
	TYPE_ELEMENT
} VENG_ParentType;

typedef struct VENG_Element VENG_Element; // Forward declaration

typedef struct VENG_Layout
{
	uint8_t arrangement;
	uint8_t align_horizontal;
	uint8_t align_vertical;

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

typedef enum Arrangement
{
	HORIZONTAL,
	VERTICAL
} Arrangement;

typedef enum Align
{
	LEFT,
	TOP,
	CENTER,
	RIGHT,
	BOTTOM
} Align;

// Listeners


// VENG core functions
/*
* Creates a layout
* Parameters: All the Layout's fields
*/
VENG_Layout VENG_Layout_Create(uint8_t arrangement, uint8_t align_horizontal, uint8_t align_vertical, VENG_Element** sub_elements, size_t sub_elements_size);

/*
* Creates a screen
* Parameters: All the screen's fields
*/
VENG_Screen VENG_Screen_Create(char* title, SDL_Surface* icon, VENG_Layout layout);

/*
* Creates an element
* Parameters: All the element's fields
*/
VENG_Element VENG_Element_Create(float w, float h, bool stretch_size, bool visible, VENG_Layout layout);

/*
* Loads a PNG into a SDL_Surface
* Parameters: the PNG's path
*/
SDL_Surface* LoadPNG (const char* path);

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
* Parameters: A pointer to a VENG_Screen structure
*/
void VENG_Init(VENG_Screen* screen); // Passing pointer to avoid the structure to be duplicated

/*
* Shuts down the SDL subsystems
* Parameters: none
*/
void VENG_Destroy();

/*
* Gets the current screen pointer
* Parameters: none
*/
VENG_Screen* VENG_GetScreen();

/*
* Gets the current window pointer
* Parameters: none
*/
SDL_Window* VENG_GetWindow();

/*
* Gets the current screen pointer
* Parameters: none
*/
SDL_Renderer* VENG_GetRenderer();

#endif