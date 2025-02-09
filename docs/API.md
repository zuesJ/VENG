# VENG API
## The Official VENG Documentation
#### VENG *(Visual Engine)* is a **lightweight C GUI** library made specifically to enhance the SDL UI experience. It's a **simple**, yet **extremely customizable** library, crafted for developers who want **full control** over every detail of their GUI.

#### In this place lays every function, structure, and tool that interacts with VENG (Visual Engine). This documentation contains accurate explanations and functionality about how this library works under the hood. 

#### In this guide, we will cover:
* **Initialization and Destruction of VENG:** How to set up an Environment and take down VENG.
* **Screens, Elements and Layouts:** The basic Building Blocks of the UI and its functioning.
* **Performance improving:** A brief overview about how VENG can run faster.
* **Element Painting:** How to actually Draw into those Elements (and why it's vastly customizable).
* **Keyboard and Mouse Listeners:** Handling user Interactions with Elements.
* **Util functions to Work with VENG:** A set of functions that improves improve your workflow with VENG.




















## 1. Initialization and Destruction of VENG:
#### To **set up** and **tear down** a **VENG** environment, you need to know a struct and some few functions:

```
typedef struct VENG_Driver
{
	SDL_Window* window;
	SDL_Renderer* renderer;
} VENG_Driver;
```
#### **Description**: This struct needs a pointer to a SDL_Window and SDL_Render variables. VENG will use that window and renderer to draw things onto the screen.

#

### `int VENG_Init(VENG_Driver driver)`
#### **Description**: This function calls every SDL sub-system that VENG needs to work properly.
#### **Parameters**: A VENG_Driver struct
#### **Returns**: an integer, 0 if no errors occurred, -1 if it failed.
#### **Usage**: A VENG_Driver is passed and sets that driver as the main rendering environment.
#### **Notes**: It calls SDL_INIT_VIDEO, SDL_INIT_TIMER and IMG_INIT_PNG (from SDL_Image). If one of those subsystems is already active, nothing will happen as SDL keeps track of every subsystem and will prevent a doble initialization of the same subsytem twice.

#

### `void VENG_Destroy(bool closeSDL)`
#### **Description**: It deletes the current driver, and if wanted, tears down SDL.
#### **Parameters**: a boolean, true if you want to tear down SDL, false to just reset VENG.
#### **Returns**: Nothing.
#### **Notes**: It will also tear down the SDL_IMG environment.
#### **Bugs**: At the moment it just resets every variable that exists in VENG.c, this won't reset listeners. (it will be fixed when the VENG listeners finish developing)



















## 2. Screens, Elements and Layouts:
#### In this section we will talk about the building blocks of VENG, its core functionality.

### <u>The building blocks:</u>

### Structs:


### Enums:
```
typedef enum VENG_Arrangement
{
	VENG_HORIZONTAL,
	VENG_VERTICAL
} VENG_Arrangement;
```
**VENG_Arrangement** is a property that needs to be assigned to a screen or an element to let VENG know how its sub-elements should be arrenged. For instance, if we are using **VENG_HORIZONTAL** and we add 2 sub-elements, those will be arranged one behind the other <u>horizontally</u>, the right side of the first box will collide into the left side of the second element.

#

```
typedef enum VENG_Align
{
	VENG_LEFT,
	VENG_TOP,
	VENG_CENTER,
	VENG_RIGHT,
	VENG_BOTTOM
} VENG_Align;
```
**VENG_Align** is yet another useful resource to build better UIs. It is assigned also to a screen or an element. It tells VENG to align sub-elements either into the center, left and right in horizontal contexts, and top and bottom in vertical contexts. If we pick up the example in VENG_Arrangement explanation, if we set the horizontal alignment to be set to right, the left side of the first box, will collide into the right side of the second element at the right corner of the element.

#

#### There is also this one extra enum that its only used internally (to identify if a void* is a Screen* or an Element*):
```
typedef enum VENG_ParentType
{
	VENG_TYPE_SCREEN,
	VENG_TYPE_ELEMENT
} VENG_ParentType;
```
















## 3. Performance improving:



















## 4. Element Painting:



















## 5. Keyboard and Mouse Listeners:



















## 6. Util functions to Work with VENG:

#

#

#



















# Writing layouts (temporary)
> Function explanation default fields (for writing the docs)
### `(a cool return type) a cool function(with some cool parameters) without';'`
#### **Description**:
#### **Parameters**:
#### **Returns**:
#### **Usage**:
#### **Notes**: