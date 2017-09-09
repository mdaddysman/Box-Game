/*
* Box Game - shell.c
* Box Game to demo SDL2 and simple AI
* This file includes functions for game shell 
* Forked from SDL Learning repo on 9/3/2017
* By Matthew K. Daddysman
*/

#include "main.h"

enum MenuOptions gMenuOptions; 
enum CurrentMenu gCurrentMenu;
SDL_Texture *gtTitle, *gtMenuOptions[numMenuOptions], *gtMenuOptionsSel[numMenuOptions], *gtVersionText[2];
SDL_Rect gTitle_rect, gMenuOptions_rect[numMenuOptions], gVersion_rect[2];
SDL_Texture *gtBack, *gtBackSel, *gtHowToPlay, *gtHighScores, *gtOptions;
SDL_Rect gBack_rect, gHowToPlay_rect, gHighScores_rect, gOptions_rect;
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

	gtBack = NULL;
	gtBackSel = NULL;
	gtHowToPlay = NULL;
	gtHighScores = NULL;
	gtOptions = NULL;

	gtTitle = makeTextTexture(r, gLargeFont, GAME_NAME, TEXT_COLOR, BG_COLOR, BLENDED);
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

	sprintf_s(buffer, sizeof(buffer), "v %d.%03d", BUILD_NUMBER / 1000 ,BUILD_NUMBER % 1000);	
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

	gtHowToPlay = makeTextTexture(r, gLargeFont, "How to Play", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtHowToPlay, NULL, NULL, &w, &h);
	gHowToPlay_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gHowToPlay_rect.y = 10;
	gHowToPlay_rect.w = w;
	gHowToPlay_rect.h = h;
	gtHighScores = makeTextTexture(r, gLargeFont, "High Scores", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtHighScores, NULL, NULL, &w, &h);
	gHighScores_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gHighScores_rect.y = 10;
	gHighScores_rect.w = w;
	gHighScores_rect.h = h;
	gtOptions = makeTextTexture(r, gLargeFont, "Options", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtOptions, NULL, NULL, &w, &h);
	gOptions_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gOptions_rect.y = 10;
	gOptions_rect.w = w;
	gOptions_rect.h = h;
	
	gtBack = makeTextTexture(r, gMenuFont, "BACK", TEXT_COLOR, BG_COLOR, BLENDED);
	gtBackSel = makeTextTexture(r, gMenuFont, "BACK", Select_Color, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtBack, NULL, NULL, &w, &h);
	gBack_rect.w = w;
	gBack_rect.h = h;
	gBack_rect.y = SCREEN_HEIGHT - gBack_rect.h - 10;
	gBack_rect.x = SCREEN_WIDTH / 2 - gBack_rect.w / 2; 



	gMenuMusic = Mix_LoadMUS(MENU_MUSIC);

	for (i = 0; i < NUM_MENU_BOXES; i++)
	{
		gMenuBoxes[i].x = rnd(SCREEN_WIDTH - 20) + 10;
		gMenuBoxes[i].y = rnd(SCREEN_HEIGHT - 20) + 10;
		gMenuBoxes[i].w = MENU_BOX_SIZE;
		gMenuBoxes[i].h = MENU_BOX_SIZE;
		gMenuBoxes[i].direction = rnd(numMoveDirection);
		gMenuBoxes[i].rand_direction = 1 + (i % 20);
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
	gCurrentMenu = MAIN_MENU;
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

	if (gtBack != NULL)
		SDL_DestroyTexture(gtBack);

	if (gtBackSel != NULL)
		SDL_DestroyTexture(gtBackSel);

	if (gtHowToPlay != NULL)
		SDL_DestroyTexture(gtHowToPlay);

	if (gtHighScores != NULL)
		SDL_DestroyTexture(gtHighScores);

	if (gtOptions != NULL)
		SDL_DestroyTexture(gtOptions);

	if (gMenuMusic != NULL)
		Mix_FreeMusic(gMenuMusic);

	if (gClickSound != NULL)
		Mix_FreeChunk(gClickSound);
}

bool shellKeyboard(SDL_Event *e, SDL_Renderer *r)
{
	bool quit = false;
	switch (gCurrentMenu)
	{
	case MAIN_MENU:
		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gMenuOptions > 0)
				gMenuOptions--;
			break;
		case SDLK_DOWN:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gMenuOptions < numMenuOptions - 1)
				gMenuOptions++;
			break;
		case SDLK_RETURN:
			switch (gMenuOptions)
			{
			case STARTGAME:
				MoveToGame(r);
				Mix_HaltMusic();
				gMenuOptions = STARTGAME;
				gCurrentMenu = MAIN_MENU;
				break;
			case HOWTOPLAY:
				Mix_PlayChannel(-1, gClickSound, 0);
				gCurrentMenu = HOWTOPLAY_MENU;
				break;
			case HIGHSCORES:
				Mix_PlayChannel(-1, gClickSound, 0);
				gCurrentMenu = HIGHSCORES_MENU;
				break;
			case OPTIONS:
				Mix_PlayChannel(-1, gClickSound, 0);
				gCurrentMenu = OPTIONS_MENU;
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
		break;
	case HOWTOPLAY_MENU:
	case HIGHSCORES_MENU:
		if (e->key.keysym.sym == SDLK_RETURN)
		{
			Mix_PlayChannel(-1, gClickSound, 0);
			gCurrentMenu = MAIN_MENU;
		}
		break;
	case OPTIONS_MENU:
		switch (e->key.keysym.sym)
		{
		case SDLK_RETURN:
			Mix_PlayChannel(-1, gClickSound, 0);
			gCurrentMenu = MAIN_MENU;
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

	if (gCurrentMenu != HOWTOPLAY_MENU)
		for (i = 0; i < NUM_MENU_BOXES; i++)
			moveShellAIBox(&gMenuBoxes[i]);
	
}

void drawShell(SDL_Renderer *r)
{
	int i;
	SDL_Rect tempbox;

	if (Mix_PlayingMusic() == 0)
		Mix_PlayMusic(gMenuMusic, -1);

	if (gCurrentMenu != HOWTOPLAY_MENU)
		for (i = 0; i < NUM_MENU_BOXES; i++)
			DrawBox(r, copyToSDLRect(&gMenuBoxes[i], &tempbox), gMenuBoxes[i].color);

	switch (gCurrentMenu)
	{
	case MAIN_MENU:
		SDL_RenderCopy(r, gtTitle, NULL, &gTitle_rect);
		for (i = 0; i < 2; i++)
			SDL_RenderCopy(r, gtVersionText[i], NULL, &gVersion_rect[i]);

		for (i = 0; i < numMenuOptions; i++)
		{
			if (i == gMenuOptions)
			{
				tempbox.w = 10;
				tempbox.h = 10;
				tempbox.y = gMenuOptions_rect[i].y + gMenuOptions_rect[i].h / 2 - tempbox.h / 2 + 1;
				tempbox.x = gMenuOptions_rect[i].x - tempbox.w - 10;
				DrawBox(r, &tempbox, WHITE);
				tempbox.x = gMenuOptions_rect[i].x + gMenuOptions_rect[i].w + 10;
				DrawBox(r, &tempbox, WHITE);
				SDL_RenderCopy(r, gtMenuOptionsSel[i], NULL, &gMenuOptions_rect[i]);
			}
			else
				SDL_RenderCopy(r, gtMenuOptions[i], NULL, &gMenuOptions_rect[i]);
		}
		break;
	case HOWTOPLAY_MENU:
		SDL_RenderCopy(r, gtHowToPlay, NULL, &gHowToPlay_rect); //draw title
		//draw back button
		tempbox.w = 10;
		tempbox.h = 10;
		tempbox.y = gBack_rect.y + gBack_rect.h / 2 - tempbox.h / 2 + 1;
		tempbox.x = gBack_rect.x - tempbox.w - 10;
		DrawBox(r, &tempbox, WHITE);
		tempbox.x = gBack_rect.x + gBack_rect.w + 10;
		DrawBox(r, &tempbox, WHITE);
		SDL_RenderCopy(r, gtBackSel, NULL, &gBack_rect);
		break;
	case HIGHSCORES_MENU:
		SDL_RenderCopy(r, gtHighScores, NULL, &gHighScores_rect); //draw title
		//draw back button
		tempbox.w = 10;
		tempbox.h = 10;
		tempbox.y = gBack_rect.y + gBack_rect.h / 2 - tempbox.h / 2 + 1;
		tempbox.x = gBack_rect.x - tempbox.w - 10;
		DrawBox(r, &tempbox, WHITE);
		tempbox.x = gBack_rect.x + gBack_rect.w + 10;
		DrawBox(r, &tempbox, WHITE);
		SDL_RenderCopy(r, gtBackSel, NULL, &gBack_rect);
		break;
	case OPTIONS_MENU:
		SDL_RenderCopy(r, gtOptions, NULL, &gOptions_rect); //draw title
		//draw back button
		tempbox.w = 10;
		tempbox.h = 10;
		tempbox.y = gBack_rect.y + gBack_rect.h / 2 - tempbox.h / 2 + 1;
		tempbox.x = gBack_rect.x - tempbox.w - 10;
		DrawBox(r, &tempbox, WHITE);
		tempbox.x = gBack_rect.x + gBack_rect.w + 10;
		DrawBox(r, &tempbox, WHITE);
		SDL_RenderCopy(r, gtBackSel, NULL, &gBack_rect);
		break;
	default:
		break;
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