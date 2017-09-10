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
enum OptionsMenuOptions gOptionsMenuOptions; 
SDL_Texture *gtTitle, *gtMenuOptions[numMenuOptions], *gtMenuOptionsSel[numMenuOptions], *gtVersionText[3];
SDL_Rect gTitle_rect, gMenuOptions_rect[numMenuOptions], gVersion_rect[3];
SDL_Texture *gtBack, *gtBackSel, *gtHowToPlay, *gtHighScores, *gtOptions;
SDL_Rect gBack_rect, gHowToPlay_rect, gHighScores_rect, gOptions_rect;
Mix_Music *gMenuMusic;
Mix_Chunk *gClickSound; 
SDL_Texture *gtMasterVolume, *gtMusicVolume, *gtSoundVolume, *gtFPSOnOff;
SDL_Rect gMasterVolume_rect, gMusicVolume_rect, gSoundVolume_rect, gFPSOnOff_rect;
int gMasterVolume, gMusicVolume, gSoundVolume; //volume levels 
SDL_Texture *gtNameTitle, *gtLevelTitle, *gtTimeTitle, *gtRanking[10], *gtHSPlayerNames[10], *gtHSLevels[10], *gtHSTimes[10];
SDL_Rect gNameTitle_rect, gLevelTitle_rect, gTimeTitle_rect, gRanking_rect[10], gHSPlayerNames_rect[10], gHSLevels_rect[10], gHSTimes_rect[10];
int gnumNameInput, gHSRank; 
char gNameInput[MAX_NAME+20];

struct AIBox gMenuBoxes[NUM_MENU_BOXES];
static const int MENU_BOX_SIZE = 10;

struct HighScore gHighScores[10];

extern TTF_Font *gSmallFont, *gLargeFont, *gMenuFont, *gMenuFontSmall; //extern fonts
extern bool gDrawFPS; 


void loadShellResources(SDL_Renderer *r)
{
	int i, w, h; 
	char buffer[100];
	SDL_Color Select_Color = { 0, 255, 255 };
	int optionsLeftOffset = 0;

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

	gtMasterVolume = NULL;
	gtMusicVolume = NULL; 
	gtSoundVolume = NULL; 
	gtFPSOnOff = NULL;

	gtNameTitle = NULL;
	gtLevelTitle = NULL;
	gtTimeTitle = NULL;
	for (i = 0; i < 10; i++)
	{
		gtRanking[i] = NULL;
		gtHSPlayerNames[i] = NULL; 
		gtHSLevels[i] = NULL;
		gtHSTimes[i] = NULL;
	}

	gMasterVolume = MAX_VOLUME;
	gSoundVolume = MAX_VOLUME;
	gMusicVolume = MAX_VOLUME;

	gHSRank = 0;
	gnumNameInput = 0;
	strcpy(gNameInput, "");

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

	sprintf_s(buffer, sizeof(buffer), "Version %d.%03d", BUILD_NUMBER / 1000 ,BUILD_NUMBER % 1000);	
	gtVersionText[0] = makeTextTexture(r, gMenuFontSmall, buffer, TEXT_COLOR, BG_COLOR, BLENDED);	
	gtVersionText[1] = makeTextTexture(r, gMenuFontSmall,
		"Game Music Copyright 2017 by Brian Hicks", TEXT_COLOR, BG_COLOR, BLENDED);
	gtVersionText[2] = makeTextTexture(r, gMenuFontSmall, 
		"Game Code Copyright 2017 under GNU GPLv3 by Matthew K. Daddysman", TEXT_COLOR, BG_COLOR, BLENDED);
	for (i = 0; i < 3; i++)
	{
		SDL_QueryTexture(gtVersionText[i], NULL, NULL, &w, &h);
		gVersion_rect[i].w = w;
		gVersion_rect[i].h = h;
		gVersion_rect[i].x = (SCREEN_WIDTH - w) / 2;
	}
	gVersion_rect[2].y = SCREEN_HEIGHT - gVersion_rect[2].h - 4;
	gVersion_rect[1].y = gVersion_rect[2].y - gVersion_rect[1].h + 2;
	gVersion_rect[0].y = gVersion_rect[1].y - gVersion_rect[0].h + 4;
	
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

	resetHighScores(); //initalize high scores
	//end main and general menu resources

	//load the save values and set
	openSaveData();
	updateVolumes();
	//end load save values

	//load option menu resources
	gtOptions = makeTextTexture(r, gLargeFont, "Options", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtOptions, NULL, NULL, &w, &h);
	gOptions_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gOptions_rect.y = 10;
	gOptions_rect.w = w;
	gOptions_rect.h = h;

	optionsLeftOffset = 25;
	sprintf_s(buffer, sizeof(buffer), "Master Volume: %d", gMasterVolume);
	gtMasterVolume = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtMasterVolume, NULL, NULL, &w, &h);
	gMasterVolume_rect.x = optionsLeftOffset;
	gMasterVolume_rect.y = gOptions_rect.y + gOptions_rect.h + 50;
	gMasterVolume_rect.w = w;
	gMasterVolume_rect.h = h;

	sprintf_s(buffer, sizeof(buffer), "Music Volume: %d", gMusicVolume);
	gtMusicVolume = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtMusicVolume, NULL, NULL, &w, &h);
	gMusicVolume_rect.x = optionsLeftOffset + 25;
	gMusicVolume_rect.y = gMasterVolume_rect.y + gMasterVolume_rect.h + 2;
	gMusicVolume_rect.w = w;
	gMusicVolume_rect.h = h;

	sprintf_s(buffer, sizeof(buffer), "Sound Volume: %d", gSoundVolume);
	gtSoundVolume = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtSoundVolume, NULL, NULL, &w, &h);
	gSoundVolume_rect.x = optionsLeftOffset + 25;
	gSoundVolume_rect.y = gMusicVolume_rect.y + gMusicVolume_rect.h + 2;
	gSoundVolume_rect.w = w;
	gSoundVolume_rect.h = h;

	if (gDrawFPS)
		sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): On");
	else
		sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): Off");
	gtFPSOnOff = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtFPSOnOff, NULL, NULL, &w, &h);
	gFPSOnOff_rect.x = optionsLeftOffset;
	gFPSOnOff_rect.y = gSoundVolume_rect.y + gSoundVolume_rect.h + 25;
	gFPSOnOff_rect.w = w;
	gFPSOnOff_rect.h = h;
	//end option menu

	//load how to play menu resources
	gtHowToPlay = makeTextTexture(r, gLargeFont, "How to Play", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtHowToPlay, NULL, NULL, &w, &h);
	gHowToPlay_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gHowToPlay_rect.y = 10;
	gHowToPlay_rect.w = w;
	gHowToPlay_rect.h = h;
	//end how to play menu

	//load high scores menu resources
	gtHighScores = makeTextTexture(r, gLargeFont, "High Scores", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtHighScores, NULL, NULL, &w, &h);
	gHighScores_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gHighScores_rect.y = 10;
	gHighScores_rect.w = w;
	gHighScores_rect.h = h;

	gtNameTitle = makeTextTexture(r, gMenuFont, "Player", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtNameTitle, NULL, NULL, &w, &h);
	gNameTitle_rect.x = 75;
	gNameTitle_rect.y = gHighScores_rect.y + gHighScores_rect.h + 5;
	gNameTitle_rect.w = w;
	gNameTitle_rect.h = h;

	gtLevelTitle = makeTextTexture(r, gMenuFont, "Level", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtLevelTitle, NULL, NULL, &w, &h);
	gLevelTitle_rect.x = gNameTitle_rect.x + 6 * gNameTitle_rect.w;
	gLevelTitle_rect.y = gNameTitle_rect.y;
	gLevelTitle_rect.w = w;
	gLevelTitle_rect.h = h;

	gtTimeTitle = makeTextTexture(r, gMenuFont, "Time", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtTimeTitle, NULL, NULL, &w, &h);
	gTimeTitle_rect.x = SCREEN_WIDTH - w - 10;
	gTimeTitle_rect.y = gNameTitle_rect.y;
	gTimeTitle_rect.w = w;
	gTimeTitle_rect.h = h;

	gtRanking[0] = makeTextTexture(r, gMenuFont, "1", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtRanking[0], NULL, NULL, &w, &h);
	gRanking_rect[0].x = 25;
	gRanking_rect[0].y = gNameTitle_rect.y + gNameTitle_rect.h + 5;
	gRanking_rect[0].w = w;
	gRanking_rect[0].h = h;

	for (i = 1; i < 10; i++)
	{
		sprintf_s(buffer, sizeof(buffer), "%d", i + 1);
		gtRanking[i] = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
		SDL_QueryTexture(gtRanking[i], NULL, NULL, &w, &h);
		gRanking_rect[i].x = gRanking_rect[0].x;
		gRanking_rect[i].y = gRanking_rect[i - 1].y + gRanking_rect[i - 1].h - 8;
		gRanking_rect[i].w = w;
		gRanking_rect[i].h = h;
	}
	//end high scores menu


	//set default menu options
	gMenuOptions = STARTGAME;
	gCurrentMenu = MAIN_MENU;
}

void freeShellResources(void)
{
	int i;

	if (gtTitle != NULL)
		SDL_DestroyTexture(gtTitle);

	for (i = 0; i < 3; i++)
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

	if (gtMasterVolume != NULL)
		SDL_DestroyTexture(gtMasterVolume);

	if (gtMusicVolume != NULL)
		SDL_DestroyTexture(gtMusicVolume);

	if (gtSoundVolume != NULL)
		SDL_DestroyTexture(gtSoundVolume);

	if (gtFPSOnOff != NULL)
		SDL_DestroyTexture(gtFPSOnOff);

	if (gtNameTitle != NULL)
		SDL_DestroyTexture(gtNameTitle);

	if (gtLevelTitle != NULL)
		SDL_DestroyTexture(gtLevelTitle);

	if (gtTimeTitle != NULL)
		SDL_DestroyTexture(gtTimeTitle);
	
	for (i = 0; i < 10; i++)
	{
		if (gtRanking[i] != NULL)
			SDL_DestroyTexture(gtRanking[i]);

		if (gtHSPlayerNames[i] != NULL)
			SDL_DestroyTexture(gtHSPlayerNames[i]);

		if (gtHSLevels[i] != NULL)
			SDL_DestroyTexture(gtHSLevels[i]);

		if (gtHSTimes[i] != NULL)
			SDL_DestroyTexture(gtHSTimes[i]);
	}

	if (gMenuMusic != NULL)
		Mix_FreeMusic(gMenuMusic);

	if (gClickSound != NULL)
		Mix_FreeChunk(gClickSound);
}

void openHighScoresScreen(SDL_Renderer *r)
{
	int i, w, h, m, s;
	char buffer[10];
	//first clean up the old textures - if they exist 
	for (i = 0; i < 10; i++)
	{
		if (gtHSPlayerNames[i] != NULL)
			SDL_DestroyTexture(gtHSPlayerNames[i]);

		if (gtHSLevels[i] != NULL)
			SDL_DestroyTexture(gtHSLevels[i]);

		if (gtHSTimes[i] != NULL)
			SDL_DestroyTexture(gtHSTimes[i]);
	}

	//now loop through the high scores structure and make the textures
	for (i = 0; i < 10; i++)
	{
		if (gHighScores[i].level < 1) //if it is an empty score
			break; //end the loop
		else
		{
			if (gCurrentMenu == ENTERNAME_MENU && i == gHSRank)
				gtHSPlayerNames[i] = makeTextTexture(r, gMenuFont, "_", TEXT_COLOR, BG_COLOR, BLENDED);
			else
				gtHSPlayerNames[i] = makeTextTexture(r, gMenuFont, gHighScores[i].Name, TEXT_COLOR, BG_COLOR, BLENDED);
			SDL_QueryTexture(gtHSPlayerNames[i], NULL, NULL, &w, &h);
			gHSPlayerNames_rect[i].x = gNameTitle_rect.x;
			gHSPlayerNames_rect[i].y = gRanking_rect[i].y;
			gHSPlayerNames_rect[i].w = w;
			gHSPlayerNames_rect[i].h = h;

			sprintf_s(buffer, sizeof(buffer), "%d", gHighScores[i].level);
			gtHSLevels[i] = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
			SDL_QueryTexture(gtHSLevels[i], NULL, NULL, &w, &h);
			gHSLevels_rect[i].x = gLevelTitle_rect.x + gLevelTitle_rect.w - w;
			gHSLevels_rect[i].y = gRanking_rect[i].y;
			gHSLevels_rect[i].w = w;
			gHSLevels_rect[i].h = h;

			m = gHighScores[i].time / 60;
			s = gHighScores[i].time - m * 60;
			sprintf_s(buffer, sizeof(buffer), "%02d:%02d", m, s);
			gtHSTimes[i] = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
			SDL_QueryTexture(gtHSTimes[i], NULL, NULL, &w, &h);
			gHSTimes_rect[i].x = gTimeTitle_rect.x + gTimeTitle_rect.w - w;
			gHSTimes_rect[i].y = gRanking_rect[i].y;
			gHSTimes_rect[i].w = w;
			gHSTimes_rect[i].h = h;
		}

	}
}

bool shellKeyboard(SDL_Event *e, SDL_Renderer *r)
{
	char nbuffer[MAX_NAME+20];
	int w, h;
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
				openHighScoresScreen(r);
				gCurrentMenu = HIGHSCORES_MENU;
				break;
			case OPTIONS:
				Mix_PlayChannel(-1, gClickSound, 0);
				gOptionsMenuOptions = BACK_OPTIONS;
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
		case SDLK_UP:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gOptionsMenuOptions > 0)
				gOptionsMenuOptions--;
			break;
		case SDLK_DOWN:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gOptionsMenuOptions < BACK_OPTIONS)
				gOptionsMenuOptions++;
			break;
		case SDLK_RETURN:
			if (gOptionsMenuOptions == BACK_OPTIONS)
			{
				Mix_PlayChannel(-1, gClickSound, 0);
				writeGameDataFile();
				gCurrentMenu = MAIN_MENU;
			}
			else if (gOptionsMenuOptions == FPSONOFF)
			{
				Mix_PlayChannel(-1, gClickSound, 0);
				gDrawFPS = !gDrawFPS;
			}
		case SDLK_LEFT:
			switch (gOptionsMenuOptions)
			{
			case MASTERVOLUME:
				if (gMasterVolume > 0)
				{
					gMasterVolume--;
					updateVolumes();
					Mix_PlayChannel(-1, gClickSound, 0);
				}
				break;
			case MUSICVOLUME:
				if (gMusicVolume > 0)
				{
					gMusicVolume--;
					updateVolumes();
				}
				break;
			case SOUNDVOLUME:
				if (gSoundVolume > 0)
				{
					gSoundVolume--;
					updateVolumes();
					Mix_PlayChannel(-1, gClickSound, 0);
				}
				break;
			case FPSONOFF:
				Mix_PlayChannel(-1, gClickSound, 0);
				gDrawFPS = !gDrawFPS;
				break;
			}
			break;
		case SDLK_RIGHT:
			switch (gOptionsMenuOptions)
			{
			case MASTERVOLUME:
				if (gMasterVolume < MAX_VOLUME)
				{
					gMasterVolume++;
					updateVolumes();
					Mix_PlayChannel(-1, gClickSound, 0);
				}
				break;
			case MUSICVOLUME:
				if (gMusicVolume < MAX_VOLUME)
				{
					gMusicVolume++;
					updateVolumes();
				}
				break;
			case SOUNDVOLUME:
				if (gSoundVolume < MAX_VOLUME)
				{
					gSoundVolume++;
					updateVolumes();
					Mix_PlayChannel(-1, gClickSound, 0);
				}
				break;
			case FPSONOFF:
				Mix_PlayChannel(-1, gClickSound, 0);
				gDrawFPS = !gDrawFPS;
				break;
			}
			break;
		default:
			break;
		}
		break;
	case ENTERNAME_MENU:
		if (e->key.keysym.sym == SDLK_RETURN && gnumNameInput > 0)
		{
			strcpy(gHighScores[gHSRank].Name, gNameInput);
			gCurrentMenu = MAIN_MENU;
			gMenuOptions = STARTGAME;
			SDL_StopTextInput();
			writeGameDataFile();
		}
		if (e->key.keysym.sym == SDLK_BACKSPACE && gnumNameInput > 0)
		{
			gnumNameInput--;
			gNameInput[gnumNameInput] = 0; //move back the null pointer to back up the string
			if (gtHSPlayerNames[gHSRank] != NULL)
				SDL_DestroyTexture(gtHSPlayerNames[gHSRank]);
			sprintf_s(nbuffer, sizeof(nbuffer), "%s_", gNameInput);
			gtHSPlayerNames[gHSRank] = makeTextTexture(r, gMenuFont, nbuffer, TEXT_COLOR, BG_COLOR, BLENDED);
			SDL_QueryTexture(gtHSPlayerNames[gHSRank], NULL, NULL, &w, &h);
			gHSPlayerNames_rect[gHSRank].w = w;
			gHSPlayerNames_rect[gHSRank].h = h;
		}
		break;
	default:
		break;
	}

	return(quit);
}

void acceptNameInput(SDL_Event *e, SDL_Renderer *r)
{
	char nbuffer[MAX_NAME+20];
	int w, h;

	if (gnumNameInput < MAX_NAME)
	{
		if (gtHSPlayerNames[gHSRank] != NULL)
			SDL_DestroyTexture(gtHSPlayerNames[gHSRank]);

		sprintf_s(gNameInput, sizeof(gNameInput), "%s%s", gNameInput, e->text.text);
		gnumNameInput = strlen(gNameInput);
		if (gnumNameInput > MAX_NAME)
		{
			gnumNameInput = MAX_NAME;
			gNameInput[MAX_NAME] = 0;
		}

		if (gnumNameInput < MAX_NAME)
		{
			sprintf_s(nbuffer, sizeof(nbuffer), "%s_", gNameInput);
			gtHSPlayerNames[gHSRank] = makeTextTexture(r, gMenuFont, nbuffer, TEXT_COLOR, BG_COLOR, BLENDED);
		}
		else
		{
			gtHSPlayerNames[gHSRank] = makeTextTexture(r, gMenuFont, gNameInput, TEXT_COLOR, BG_COLOR, BLENDED);
		}
		SDL_QueryTexture(gtHSPlayerNames[gHSRank], NULL, NULL, &w, &h);
		gHSPlayerNames_rect[gHSRank].w = w;
		gHSPlayerNames_rect[gHSRank].h = h;
	}
}

void shellLogic(SDL_Renderer *r)
{
	int i, w, h;
	char buffer[100];
	SDL_Color Select_Color = { 0, 255, 255 };

	if (gCurrentMenu != HOWTOPLAY_MENU)
		for (i = 0; i < NUM_MENU_BOXES; i++)
			moveShellAIBox(&gMenuBoxes[i]);

	if (gCurrentMenu == OPTIONS_MENU)
	{
		if (gtMasterVolume != NULL)
			SDL_DestroyTexture(gtMasterVolume);
		sprintf_s(buffer, sizeof(buffer), "Master Volume: %d", gMasterVolume);
		if (gOptionsMenuOptions == MASTERVOLUME)
			gtMasterVolume = makeTextTexture(r, gMenuFont, buffer, Select_Color, BG_COLOR, BLENDED);
		else
			gtMasterVolume = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
		SDL_QueryTexture(gtMasterVolume, NULL, NULL, &w, &h);
		gMasterVolume_rect.w = w;
		gMasterVolume_rect.h = h;

		if (gtMusicVolume != NULL)
			SDL_DestroyTexture(gtMusicVolume);
		sprintf_s(buffer, sizeof(buffer), "Music Volume: %d", gMusicVolume);
		if (gOptionsMenuOptions == MUSICVOLUME)
			gtMusicVolume = makeTextTexture(r, gMenuFont, buffer, Select_Color, BG_COLOR, BLENDED);
		else
			gtMusicVolume = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
		SDL_QueryTexture(gtMusicVolume, NULL, NULL, &w, &h);
		gMusicVolume_rect.w = w;
		gMusicVolume_rect.h = h;

		if (gtSoundVolume != NULL)
			SDL_DestroyTexture(gtSoundVolume);
		sprintf_s(buffer, sizeof(buffer), "Sound Volume: %d", gSoundVolume);
		if (gOptionsMenuOptions == SOUNDVOLUME)
			gtSoundVolume = makeTextTexture(r, gMenuFont, buffer, Select_Color, BG_COLOR, BLENDED);
		else
			gtSoundVolume = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
		SDL_QueryTexture(gtSoundVolume, NULL, NULL, &w, &h);
		gSoundVolume_rect.w = w;
		gSoundVolume_rect.h = h;

		if (gDrawFPS)
			sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): On");
		else
			sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): Off");

		if (gtFPSOnOff != NULL)
			SDL_DestroyTexture(gtFPSOnOff);
		if (gOptionsMenuOptions == FPSONOFF)
			gtFPSOnOff = makeTextTexture(r, gMenuFont, buffer, Select_Color, BG_COLOR, BLENDED);
		else
			gtFPSOnOff = makeTextTexture(r, gMenuFont, buffer, TEXT_COLOR, BG_COLOR, BLENDED);
		SDL_QueryTexture(gtFPSOnOff, NULL, NULL, &w, &h);
		gFPSOnOff_rect.w = w;
		gFPSOnOff_rect.h = h;
	}
	
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
		for (i = 0; i < 3; i++)
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
	case ENTERNAME_MENU:
		SDL_RenderCopy(r, gtHighScores, NULL, &gHighScores_rect); //draw title
		SDL_RenderCopy(r, gtNameTitle, NULL, &gNameTitle_rect); //draw the name title column
		SDL_RenderCopy(r, gtLevelTitle, NULL, &gLevelTitle_rect); //draw the level title column
		SDL_RenderCopy(r, gtTimeTitle, NULL, &gTimeTitle_rect); //draw the time title column
		for (i = 0; i < 10; i++)
			SDL_RenderCopy(r, gtRanking[i], NULL, &gRanking_rect[i]); //draw the numbers
		for (i = 0; i < 10; i++) //draw the high scores
		{
			if (gtHSPlayerNames[i] != NULL)
				SDL_RenderCopy(r, gtHSPlayerNames[i], NULL, &gHSPlayerNames_rect[i]);

			if (gtHSLevels[i] != NULL)
				SDL_RenderCopy(r, gtHSLevels[i], NULL, &gHSLevels_rect[i]);

			if (gtHSTimes[i] != NULL)
				SDL_RenderCopy(r, gtHSTimes[i], NULL, &gHSTimes_rect[i]);
		}
		if (gCurrentMenu == HIGHSCORES_MENU)
		{
			//draw back button
			tempbox.w = 10;
			tempbox.h = 10;
			tempbox.y = gBack_rect.y + gBack_rect.h / 2 - tempbox.h / 2 + 1;
			tempbox.x = gBack_rect.x - tempbox.w - 10;
			DrawBox(r, &tempbox, WHITE);
			tempbox.x = gBack_rect.x + gBack_rect.w + 10;
			DrawBox(r, &tempbox, WHITE);
			SDL_RenderCopy(r, gtBackSel, NULL, &gBack_rect);
		}
		break;
	case OPTIONS_MENU:
		SDL_RenderCopy(r, gtOptions, NULL, &gOptions_rect); //draw title
		SDL_RenderCopy(r, gtMasterVolume, NULL, &gMasterVolume_rect);
		SDL_RenderCopy(r, gtMusicVolume, NULL, &gMusicVolume_rect);
		SDL_RenderCopy(r, gtSoundVolume, NULL, &gSoundVolume_rect);
		SDL_RenderCopy(r, gtFPSOnOff, NULL, &gFPSOnOff_rect);
		//draw back button
		if (gOptionsMenuOptions == BACK_OPTIONS)
			SDL_RenderCopy(r, gtBackSel, NULL, &gBack_rect);
		else
			SDL_RenderCopy(r, gtBack, NULL, &gBack_rect);
		//draw boxes around selected option
		tempbox.w = 10;
		tempbox.h = 10;
		switch (gOptionsMenuOptions)
		{
		case MASTERVOLUME:
			tempbox.y = gMasterVolume_rect.y + gMasterVolume_rect.h / 2 - tempbox.h / 2 + 1;
			tempbox.x = gMasterVolume_rect.x - tempbox.w - 10;
			DrawBox(r, &tempbox, WHITE);
			tempbox.x = gMasterVolume_rect.x + gMasterVolume_rect.w + 10;
			DrawBox(r, &tempbox, WHITE);
			break;
		case MUSICVOLUME:
			tempbox.y = gMusicVolume_rect.y + gMusicVolume_rect.h / 2 - tempbox.h / 2 + 1;
			tempbox.x = gMusicVolume_rect.x - tempbox.w - 10;
			DrawBox(r, &tempbox, WHITE);
			tempbox.x = gMusicVolume_rect.x + gMusicVolume_rect.w + 10;
			DrawBox(r, &tempbox, WHITE);
			break;
		case SOUNDVOLUME:
			tempbox.y = gSoundVolume_rect.y + gSoundVolume_rect.h / 2 - tempbox.h / 2 + 1;
			tempbox.x = gSoundVolume_rect.x - tempbox.w - 10;
			DrawBox(r, &tempbox, WHITE);
			tempbox.x = gSoundVolume_rect.x + gSoundVolume_rect.w + 10;
			DrawBox(r, &tempbox, WHITE);
			break;
		case FPSONOFF:
			tempbox.y = gFPSOnOff_rect.y + gFPSOnOff_rect.h / 2 - tempbox.h / 2 + 1;
			tempbox.x = gFPSOnOff_rect.x - tempbox.w - 10;
			DrawBox(r, &tempbox, WHITE);
			tempbox.x = gFPSOnOff_rect.x + gFPSOnOff_rect.w + 10;
			DrawBox(r, &tempbox, WHITE);
			break;
		case BACK_OPTIONS:
			tempbox.y = gBack_rect.y + gBack_rect.h / 2 - tempbox.h / 2 + 1;
			tempbox.x = gBack_rect.x - tempbox.w - 10;
			DrawBox(r, &tempbox, WHITE);
			tempbox.x = gBack_rect.x + gBack_rect.w + 10;
			DrawBox(r, &tempbox, WHITE);
			break;
		default:
			break;
		}		
		break;
	default:
		break;
	}
}

void checkIfNewHighScore(int levelAchieved, unsigned long int time, SDL_Renderer *r)
{
	int i;
	int scoreSlot = -1;
	//char buffer[10];

	if (levelAchieved < 1) //didn't finish a level don't do anything
	{
		gMenuOptions = STARTGAME;
		gCurrentMenu = MAIN_MENU;
	}
	else //finished a level, now check if any of the scores are beaten 
	{
		for (i = 0; i < 10; i++)
		{
			if (levelAchieved > gHighScores[i].level) //slot the score in here
			{
				scoreSlot = i;
				break;
			}
			else if (levelAchieved == gHighScores[i].level) //same level check to see if the time is lower as a tie breaker
			{
				if (time <= gHighScores[i].time)
				{
					scoreSlot = i;
					break;
				}
			}
		} //end find the score slot

		if (scoreSlot > -1) //if a new high score move the high scores and slot in the new one
		{
			for (i = 9; i > scoreSlot; i--)
			{
				gHighScores[i].level = gHighScores[i - 1].level;
				gHighScores[i].time = gHighScores[i - 1].time;
				strcpy(gHighScores[i].Name, gHighScores[i - 1].Name);
			}

			gHighScores[scoreSlot].level = levelAchieved;
			gHighScores[scoreSlot].time = time;
			//sprintf_s(buffer, sizeof(buffer), "Player%d",scoreSlot);
			strcpy(gHighScores[scoreSlot].Name, "");
			gnumNameInput = 0;
			strcpy(gNameInput, "");
			gHSRank = scoreSlot;
			gCurrentMenu = ENTERNAME_MENU;
			openHighScoresScreen(r);
			SDL_StartTextInput(); //enable text input
		}
		else
		{
			//just return to main menu
			gMenuOptions = STARTGAME;
			gCurrentMenu = MAIN_MENU;
		}
	}
}

void updateVolumes(void)
{
	int musicvol, soundvol; 

	musicvol = (int)(gMasterVolume * gMusicVolume * MIX_MAX_VOLUME / 10000);
	soundvol = (int)(gMasterVolume * gSoundVolume * MIX_MAX_VOLUME / 10000);

#ifdef _DEBUG_BUILD_
	printf("Volume changed. Music: %d / %d, Sound: %d / %d\n", musicvol, MIX_MAX_VOLUME, soundvol, MIX_MAX_VOLUME);
#endif
	Mix_VolumeMusic(musicvol);
	Mix_Volume(-1, soundvol);
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

void openSaveData(void)
{
	int i;
	FILE *datafile;
	int master, music, sound;
	char name[MAX_NAME+1], buffer[25];
	int level, namelen;
	unsigned long int time;
	bool success = true;

	datafile = fopen(SAVE_DATA,"r");

	if (datafile == NULL)
	{

		printf("Save data file: %s not found. Creating new file.\n",SAVE_DATA);

		writeGameDataFile();
	}
	else
	{

		printf("Save data loaded.\n");

		master = -1; music = -1; sound = -1;
		fscanf(datafile, "%d, %d, %d", &master, &music, &sound);
		fgets(buffer, 25, datafile); //grab the newline char to clean up 
		printf("Found: %d, %d, %d\n", master, music, sound);
		if (master > MAX_VOLUME || master < 0 ||
			music > MAX_VOLUME || music < 0 ||
			sound > MAX_VOLUME || sound < 0)
		{

			printf("Save data corrupt. Making new file.\n");

			writeGameDataFile();
			success = false;
		}

		if (success)
		{
			for (i = 0; i < 10; i++)
			{
				level = -1; time = 0;
				fgets(name, MAX_NAME+1, datafile);
				namelen = strlen(name);
				name[namelen-1] = 0; //remove the newline 
				fscanf(datafile, "%d, %d", &level, &time); //grab the \n char 
				fgets(buffer, 20, datafile);
				if (level < 0)
				{
					printf("Save data corrupt. Making new file.\n");
					resetHighScores();
					writeGameDataFile();
					success = false;
					break;
				}
				printf("%d: Name: %s, Level: %d, Time: %d\n", i + 1, name, level, time);
				strcpy(gHighScores[i].Name, name);
				gHighScores[i].level = level;
				gHighScores[i].time = time;
			}
		}

		if (success)
		{
			gMasterVolume = master;
			gMusicVolume = music;
			gSoundVolume = sound;
		}
	}

	if (datafile != NULL)
		fclose(datafile);
}

void writeGameDataFile(void)
{
	int i;
	FILE *datafile;

	datafile = fopen(SAVE_DATA, "w");
	if (datafile == NULL)
		printf("Cannot write to data file: %s\n", SAVE_DATA);
	else
	{
		fprintf(datafile, "%d, %d, %d\n", gMasterVolume, gMusicVolume, gSoundVolume);

		for (i = 0; i < 10; i++)
		{
			fprintf(datafile, "%s\n", gHighScores[i].Name);
			fprintf(datafile, "%d, %d\n", gHighScores[i].level, gHighScores[i].time);
		}
	}

	if (datafile != NULL)
		fclose(datafile);
}

void resetHighScores(void)
{
	int i;

	for (i = 0; i < 10; i++)
	{
		strcpy(gHighScores[i].Name,"");
		gHighScores[i].level = 0;
		gHighScores[i].time = 0;
	}
}

