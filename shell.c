/*
* Box Game - shell.c
* Box Game to demo SDL2 and simple AI
* This file includes functions for game shell 
* Forked from SDL Learning repo on 9/3/2017
* By Matthew K. Daddysman
*/

#include "main.h"

enum MenuOptions gMenuOptions; 
SDL_Texture *gtTitle, *gtMenuOptions[numMenuOptions], *gtMenuOptionsSel[numMenuOptions], *gtVersionText[2];
SDL_Rect gTitle_rect, gMenuOptions_rect[numMenuOptions], gVersion_rect[2];
Mix_Music *gMenuMusic;
Mix_Chunk *gClickSound; 

struct AIBox gMenuBoxes[NUM_MENU_BOXES];
static const int MENU_BOX_SIZE = 10;

//extern fonts
extern TTF_Font *gSmallFont, *gLargeFont, *gMenuFont, *gMenuFontSmall;

void loadShellResources(SDL_Renderer *r)
{
	int i, w, h; 
	char buffer[100];
	SDL_Color Select_Color = { 0, 255, 255 };

	gMenuMusic = NULL;
	gClickSound = NULL;
	gtTitle = NULL;
	for (i = 0; i < numMenuOptions; i++)
	{
		gtMenuOptions[i] = NULL;
		gtMenuOptionsSel[i] = NULL;
	}
	for (i = 0; i < 2; i++)
		gtVersionText[i] = NULL;

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

	gtMenuOptionsSel[0] = makeTextTexture(r, gMenuFont, "Start Game", Select_Color, BG_COLOR, BLENDED);
	gtMenuOptionsSel[1] = makeTextTexture(r, gMenuFont, "How to Play", Select_Color, BG_COLOR, BLENDED);
	gtMenuOptionsSel[2] = makeTextTexture(r, gMenuFont, "High Scores", Select_Color, BG_COLOR, BLENDED);
	gtMenuOptionsSel[3] = makeTextTexture(r, gMenuFont, "Options", Select_Color, BG_COLOR, BLENDED);
	gtMenuOptionsSel[4] = makeTextTexture(r, gMenuFont, "Exit", Select_Color, BG_COLOR, BLENDED);

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

	sprintf_s(buffer, sizeof(buffer), "v 0.%d", BUILD_NUMBER);
	
	gtVersionText[0] = makeTextTexture(r, gMenuFontSmall, buffer, TEXT_COLOR, BG_COLOR, BLENDED);	
	gtVersionText[1] = makeTextTexture(r, gMenuFontSmall, 
		"Copyright 2017 under GNU GPLv3 by Matthew K. Daddysman", TEXT_COLOR, BG_COLOR, BLENDED);
	for (i = 0; i < 2; i++)
	{
		SDL_QueryTexture(gtVersionText[i], NULL, NULL, &w, &h);
		gVersion_rect[i].w = w;
		gVersion_rect[i].h = h;
		gVersion_rect[i].x = (SCREEN_WIDTH - w) / 2;
	}
	gVersion_rect[1].y = SCREEN_HEIGHT - gVersion_rect[1].h - 4;
	gVersion_rect[0].y = gVersion_rect[1].y - gVersion_rect[0].h;

	gMenuMusic = Mix_LoadMUS(MENU_MUSIC);

	for (i = 0; i < NUM_MENU_BOXES; i++)
	{
		gMenuBoxes[i].x = rnd(SCREEN_WIDTH - 20) + 10;
		gMenuBoxes[i].y = rnd(SCREEN_HEIGHT - 20) + 10;
		gMenuBoxes[i].w = MENU_BOX_SIZE;
		gMenuBoxes[i].h = MENU_BOX_SIZE;
		gMenuBoxes[i].direction = rnd(numMoveDirection);
		gMenuBoxes[i].rand_direction = 1 * (i % 20);
		gMenuBoxes[i].speed = i / 20 + 1;
		gMenuBoxes[i].aitype = RANDOM;
		if (i == 0)
			gMenuBoxes[i].color = BLUE;
		else if (i % 5 == 0)
			gMenuBoxes[i].color = YELLOW;
		else
			gMenuBoxes[i].color = ORANGE;
	}
	gClickSound = Mix_LoadWAV(CLICK_SOUND);
	gMenuOptions = STARTGAME;
}

void freeShellResources(void)
{
	int i;

	if (gtTitle != NULL)
		SDL_DestroyTexture(gtTitle);

	for (i = 0; i < 2; i++)
	{
		if (gtVersionText[i] != NULL)
			SDL_DestroyTexture(gtVersionText[i]);
	}

	for (i = 0; i < numMenuOptions; i++)
	{
		if (gtMenuOptions[i] != NULL)
			SDL_DestroyTexture(gtMenuOptions[i]);
		if (gtMenuOptionsSel[i] != NULL)
			SDL_DestroyTexture(gtMenuOptionsSel[i]);
	}

	if (gMenuMusic != NULL)
		Mix_FreeMusic(gMenuMusic);

	if (gClickSound != NULL)
		Mix_FreeChunk(gClickSound);
}

bool shellKeyboard(SDL_Event *e, SDL_Renderer *r)
{
	bool quit = false;
	switch (e->key.keysym.sym)
	{
	case SDLK_UP:
		Mix_PlayChannel(-1, gClickSound, 0);
		if (gMenuOptions > 0)
			gMenuOptions--;
		break;
	case SDLK_DOWN:
		Mix_PlayChannel(-1, gClickSound, 0);
		if (gMenuOptions < numMenuOptions-1)
			gMenuOptions++;
		break;
	case SDLK_RETURN:
		switch (gMenuOptions)
		{
		case STARTGAME:
			MoveToGame(r);
			Mix_HaltMusic();
			gMenuOptions = STARTGAME;
			break;
		case HOWTOPLAY:
			break;
		case HIGHSCORES:
			break;
		case OPTIONS:
			break;
		case EXIT:
			quit = true;
			break;
		default: 
			break;
		}
		break;
	default:
		break;
	}

	return(quit);
}

void shellLogic(void)
{
	int i;

	for (i = 0; i < NUM_MENU_BOXES; i++)
	{
		moveShellAIBox(&gMenuBoxes[i]);
	}
}

void drawShell(SDL_Renderer *r)
{
	int i;
	SDL_Rect tempbox;

	if (Mix_PlayingMusic() == 0)
		Mix_PlayMusic(gMenuMusic, -1);

	for (i = 0; i < NUM_MENU_BOXES; i++)
		DrawBox(r, copyToSDLRect(&gMenuBoxes[i], &tempbox), gMenuBoxes[i].color);

	SDL_RenderCopy(r, gtTitle, NULL, &gTitle_rect);
	for (i = 0; i < 2; i++)
		SDL_RenderCopy(r, gtVersionText[i], NULL, &gVersion_rect[i]);

	for (i = 0; i < numMenuOptions; i++) 
	{
		if (i == gMenuOptions)
			SDL_RenderCopy(r, gtMenuOptionsSel[i], NULL, &gMenuOptions_rect[i]);
		else
			SDL_RenderCopy(r, gtMenuOptions[i], NULL, &gMenuOptions_rect[i]);
	}
}

void moveShellAIBox(struct AIBox *ai)
{
	//move the box and then see if the direction should be changed for the next move 
	changeAIBoxCoordinates(ai);
	if (rnd(100) < ai->rand_direction)
		ai->direction = rnd(numMoveDirection);

	//check if it should be moved to the other side of the screen
	if (ai->x < 0 - ai->w)
		ai->x = SCREEN_WIDTH;
	else if (ai->x > SCREEN_WIDTH)
		ai->x = 0 - ai->w;

	if (ai->y < 0 - ai->h)
		ai->y = SCREEN_HEIGHT;
	else if (ai->y > SCREEN_HEIGHT)
		ai->y = 0 - ai->h;

}