#ifndef MAIN_H
#define MAIN_H

typedef struct Label
{
	char* text;
	int text_size;
	SDL_Color bg_color;
	SDL_Color text_color;
} Label;

void draw_label (Label* label, VENG_Element* element, TTF_Font* font)
{
	SDL_Rect area = VENG_StartDrawing(element);

	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, label->bg_color.r, label->bg_color.g, label->bg_color.b, label->bg_color.a);
	SDL_RenderFillRect(VENG_GetDriver().renderer, &area);

	VENG_StopDrawing(NULL);

	TTF_SetFontSize(font, label->text_size);
	SDL_Surface* sur = TTF_RenderText_Blended(font, label->text, label->text_color);
	SDL_Texture* text = SDL_CreateTextureFromSurface(VENG_GetDriver().renderer, sur);
	int textW, textH;
	SDL_QueryTexture(text, NULL, NULL, &textW, &textH);
	SDL_RenderCopy(VENG_GetDriver().renderer, text, NULL, &element->rect);

	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, 0, 0, 0, 0);
	
	SDL_FreeSurface(sur);
	SDL_DestroyTexture(text);
}
	

void fill_a_rect_with_color (VENG_Element* element, SDL_Color color)
{
	SDL_Rect area = VENG_StartDrawing(element);

	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(VENG_GetDriver().renderer, &area);
	SDL_SetRenderDrawColor(VENG_GetDriver().renderer, 0, 0, 0, 0);

	VENG_StopDrawing(NULL);
}

#endif