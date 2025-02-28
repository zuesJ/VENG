#ifndef MAIN_H
#define MAIN_H

bool palanquisima = false;

void box2_on_click(VENG_Element* element, SDL_Event* event)
{
	palanquisima = (palanquisima) ? false : true;
}

void fill_a_rect_with_color (VENG_Element* element, SDL_Color color)
{
	SDL_Rect area = VENG_StartDrawing(element);

	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(VENG_GetDriver().renderer, &area);
	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, 0, 0, 0, 0);

	VENG_StopDrawing(NULL);
}

void tool_box_on_click (VENG_Element* element, SDL_Event* event)
{
	printf("tool_box tiene el ptr:%p\n", element);
}

#endif