#ifndef MAIN_H
#define MAIN_H

bool palanquisima = false;

void box2_on_click(VENG_Element* element, SDL_Event* event)
{
	palanquisima = (palanquisima) ? false : true;
}

void fill_a_rect_with_orange (VENG_Element* element)
{
	SDL_Rect viewport = element->rect;

	SDL_Rect area = VENG_StartDrawing(element);

	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, 255, 127, 80, 255);
	SDL_RenderFillRect(VENG_GetDriver().renderer, &viewport);
	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, 0, 0, 0, 0);

	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(VENG_GetDriver().renderer, area.x, area.y, area.w, area.h);
	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, 0, 0, 0, 0);
	VENG_StopDrawing(NULL);
}

void tool_box_on_click (VENG_Element* element, SDL_Event* event)
{
	printf("tool_box tiene el ptr:%p\n", element);
}

#endif