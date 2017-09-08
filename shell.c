/*
* Box Game - shell.c
* Box Game to demo SDL2 and simple AI
* This file includes functions for game shell 
* Forked from SDL Learning repo on 9/3/2017
* By Matthew K. Daddysman
*/

#include "main.h"

enum MenuOptions gMenuOptions; 
SDL_Texture *gtTitle, *gtMenuOptions[numMenuOptions];
SDL_Rect gTitle_rect, gMenuOptions_rect[numMenuOptions];

//extern fonts
extern TTF_Font *gSmallFont, *gLargeFont, *gMenuFont;

void loadShellResources(SDL_Renderer *r)
{
	int i, w, h; 

	gtTitle = NULL;
	for (i = 0; i < numMenuOptions; i++)
		gtMenuOptions[i] = NULL;

	gtTitle = makeTextTexture(r, gLargeFont, "STAR BOX", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtTitle, NULL, NULL, &w, &h);
	gTitle_rect.w = w;
	gTitle_rect.h = h;
	gTitle_rect.x = (SCREEN_WIDTH - w) / 2;
	gTitle_rect.y = (SCREEN_HEIGHT/2 - h) / 2;

	gtMenuOptions[0] = makeTextTexture(r, gMenuFont, "Start Game", TEXT_COLOR, BG_COLOR, BLENDED);
	gtMenuOptions[1] = makeTextTexture(r, gMenuFont, "How to Play", TEXT_COLOR, BG_COLOR, BLENDED);
	gtMenuOptions[2] = makeTextTexture(r, gMenuFont, "High Scores", TEXT_COLOR, BG_COLOR, BLENDED);
	gtMenuOptions[3] = makeTextTexture(r, gMenuFont, "Options", TEXT_COLOR, BG_COLOR, BLENDED);
	gtMenuOptions[4] = makeTextTexture(r, gMenuFont, "Exit", TEXT_COLOR, BG_COLOR, BLENDED);

	for (i = 0; i < numMenuOptions; i++)
	{
		SDL_QueryTexture(gtMenuOptions[i], NULL, NULL, &w, &h);
		gMenuOptions_rect[i].w = w;
		gMenuOptions_rect[i].h = h;
		gMenuOptions_rect[i].x = (SCREEN_WIDTH - w) / 2;
		if (i == 0)
			gMenuOptions_rect[i].y = SCREEN_HEIGHT / 2;
		else
			gMenuOptions_rect[i].y = gMenuOptions_rect[i - 1].y + gMenuOptions_rect[i - 1].h + 4;
	}

	gMenuOptions = HOWTOPLAY;
}

void freeShellResources(void)
{
	int i;

	if (gtTitle != NULL)
		SDL_DestroyTexture(gtTitle);

	for (i = 0; i < numMenuOptions; i++)
	{
		if (gtMenuOptions[i] != NULL)
			SDL_DestroyTexture(gtMenuOptions[i]);
	}
}

bool shellKeyboard(SDL_Event *e, SDL_Renderer *r)
{
	bool quit = false;


	return(quit);
}

void drawShell(SDL_Renderer *r)
{
	int i;

	SDL_RenderCopy(r, gtTitle, NULL, &gTitle_rect);

	for (i = 0; i < numMenuOptions; i++) 
	{
		SDL_RenderCopy(r, gtMenuOptions[i], NULL, &gMenuOptions_rect[i]);
	}
}