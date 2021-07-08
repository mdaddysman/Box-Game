/*
* Box Game - shell.c
* Box Game to demo SDL2 and simple AI
* This file includes functions for game shell 
* Forked from SDL Learning repo on 9/3/2017
* By Matthew K. Daddysman
*/

#include "main.h"

#define NUM_HOW_TO_PLAY 6
#define NUM_HOW_TO_KEYS 9

enum MenuOptions gMenuOptions; 
enum CurrentMenu gCurrentMenu;
enum OptionsMenuOptions gOptionsMenuOptions; 
enum ResolutionOptions gResolutionOptions;
SDL_Texture *gtTitle, *gtMenuOptions[numMenuOptions], *gtMenuOptionsSel[numMenuOptions], *gtVersionText[3];
SDL_Rect gTitle_rect, gMenuOptions_rect[numMenuOptions], gVersion_rect[3];
SDL_Texture *gtBack, *gtBackSel, *gtHowToPlay, *gtHighScores, *gtOptions;
SDL_Rect gBack_rect, gHowToPlay_rect, gHighScores_rect, gOptions_rect;
Mix_Music *gMenuMusic;
Mix_Chunk *gClickSound; 
SDL_Texture *gtMasterVolume, *gtMusicVolume, *gtSoundVolume, *gtFPSOnOff, *gtResolution[MAX_RESOLUTIONS], *gtResolutionSel[MAX_RESOLUTIONS];
SDL_Rect gMasterVolume_rect, gMusicVolume_rect, gSoundVolume_rect, gFPSOnOff_rect, gResolution_rect[MAX_RESOLUTIONS];
int gMasterVolume, gMusicVolume, gSoundVolume; //volume levels 
SDL_Texture *gtNameTitle, *gtLevelTitle, *gtTimeTitle, *gtRanking[10], *gtHSPlayerNames[10], *gtHSLevels[10], *gtHSTimes[10];
SDL_Rect gNameTitle_rect, gLevelTitle_rect, gTimeTitle_rect, gRanking_rect[10], gHSPlayerNames_rect[10], gHSLevels_rect[10], gHSTimes_rect[10];
int gnumNameInput, gHSRank; 
char gNameInput[MAX_NAME+20];
int gHowToPlayFrame; //frame counter for the how to play animations
SDL_Texture *gtHowToPlayText[NUM_HOW_TO_PLAY];
SDL_Rect grHowToPlayText[NUM_HOW_TO_PLAY], gHowToKeyboard[NUM_HOW_TO_KEYS];
SDL_Texture *gtHowToPlayKeyText[6][2];
SDL_Rect grHowToPlayKeyText[NUM_HOW_TO_KEYS];
SDL_Rect gDummyRect, gHowToBoostText, gHowToBoostMeter[2], gHowToDemoRects[5], gHowToVictory, gHowToPlayArea[2];
SDL_Texture *gtHowToHitPoints[3];
SDL_Rect grHowToHitPoints[3], grHowToGameOver;


struct AIBox gMenuBoxes[NUM_MENU_BOXES];
static const int MENU_BOX_SIZE = 10;

struct HighScore gHighScores[10];

//extern TTF_Font *gSmallFont, *gLargeFont, *gMenuFont, *gMenuFontSmall; //extern fonts
extern bool gDrawFPS; 
extern bool gUpdateResolution;
extern SDL_Texture *gtBoost, *gtvictory, *gtgameover;
extern SDL_Rect gBoost_rect, gvictory_rect, ggameover_rect;

void initalizeShellPointers(void)
{
	int i, j;
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

	for (i = 0; i < MAX_RESOLUTIONS; i++)
	{
		gtResolution[i] = NULL;
		gtResolutionSel[i] = NULL;
	}

	for (i = 0; i < NUM_HOW_TO_PLAY; i++)
		gtHowToPlayText[i] = NULL;

	for (i = 0; i < 6; i++)
		for (j = 0; j < 2; j++)
			gtHowToPlayKeyText[i][j] = NULL;

	for (i = 0; i < 3; i++)
		gtHowToHitPoints[i] = NULL;
}

void loadShellResources(SDL_Renderer *r)
{
	int i;	
	int optionsLeftOffset = 0;	

	gMasterVolume = MAX_VOLUME;
	gSoundVolume = MAX_VOLUME;
	gMusicVolume = MAX_VOLUME;

	gHSRank = 0;
	gnumNameInput = 0;
	strcpy(gNameInput, "");
	
	gTitle_rect.x = (SCREEN_WIDTH - gTitle_rect.w) / 2;
	gTitle_rect.y = (SCREEN_HEIGHT / 2 - gTitle_rect.h) / 2;
	for (i = 0; i < numMenuOptions; i++)
	{
		gMenuOptions_rect[i].x = (SCREEN_WIDTH - gMenuOptions_rect[i].w) / 2;
		if (i == 0)
			gMenuOptions_rect[i].y = SCREEN_HEIGHT / 2;
		else
			gMenuOptions_rect[i].y = gMenuOptions_rect[i - 1].y + gMenuOptions_rect[i - 1].h + 4;
	}

	for (i = 0; i < 3; i++)
		gVersion_rect[i].x = (SCREEN_WIDTH - gVersion_rect[i].w) / 2;
	
	gVersion_rect[2].y = SCREEN_HEIGHT - gVersion_rect[2].h - 4;
	gVersion_rect[1].y = gVersion_rect[2].y - gVersion_rect[1].h + 2;
	gVersion_rect[0].y = gVersion_rect[1].y - gVersion_rect[0].h + 4;
		
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

	
	//end load save values

	//load option menu resources
	
	gOptions_rect.x = SCREEN_WIDTH / 2 - gOptions_rect.w / 2;
	gOptions_rect.y = 10;


	optionsLeftOffset = 25;
	
	gMasterVolume_rect.x = optionsLeftOffset;
	gMasterVolume_rect.y = gOptions_rect.y + gOptions_rect.h + 50;
	
	gMusicVolume_rect.x = optionsLeftOffset + 25;
	gMusicVolume_rect.y = gMasterVolume_rect.y + gMasterVolume_rect.h + 2;
	
	gSoundVolume_rect.x = optionsLeftOffset + 25;
	gSoundVolume_rect.y = gMusicVolume_rect.y + gMusicVolume_rect.h + 2;
	
	gFPSOnOff_rect.x = optionsLeftOffset;
	gFPSOnOff_rect.y = gSoundVolume_rect.y + gSoundVolume_rect.h + 25;

	gResolution_rect[0].x = optionsLeftOffset;
	gResolution_rect[0].y = gFPSOnOff_rect.y + gFPSOnOff_rect.h + 25;
	
	gResolution_rect[1].x = optionsLeftOffset;
	gResolution_rect[1].y = gFPSOnOff_rect.y + gFPSOnOff_rect.h + 25;

	gResolution_rect[2].x = optionsLeftOffset;
	gResolution_rect[2].y = gFPSOnOff_rect.y + gFPSOnOff_rect.h + 25;
	//end option menu

	//load how to play menu resources
	gHowToPlay_rect.x = SCREEN_WIDTH / 2 - gHowToPlay_rect.w / 2;
	gHowToPlay_rect.y = 10;

	grHowToPlayText[0].x = SCREEN_WIDTH / 2 - grHowToPlayText[0].w / 2;
	grHowToPlayText[0].y = gHowToPlay_rect.y + gHowToPlay_rect.h + 10;

	grHowToPlayText[1].x = SCREEN_WIDTH / 4 - grHowToPlayText[1].w / 2;
	grHowToPlayText[1].y = 250;

	grHowToPlayText[2].x = SCREEN_WIDTH / 4 * 3 - grHowToPlayText[2].w / 2;
	grHowToPlayText[2].y = 250;
	grHowToPlayText[3].x = SCREEN_WIDTH / 4 * 3 - grHowToPlayText[3].w / 2;
	grHowToPlayText[3].y = grHowToPlayText[2].y + grHowToPlayText[2].h + 5;

	grHowToPlayText[4].x = SCREEN_WIDTH / 2 - grHowToPlayText[4].w / 2;
	grHowToPlayText[4].y = 407;

	grHowToPlayText[5].x = SCREEN_WIDTH / 4 - grHowToPlayText[5].w / 2;
	grHowToPlayText[5].y = grHowToPlayText[1].y + grHowToPlayText[1].h + 5;

	int keysize = 30;
	gHowToKeyboard[0].y = grHowToPlayText[0].y + grHowToPlayText[0].h + 10;
	gHowToKeyboard[0].w = keysize;
	gHowToKeyboard[0].h = keysize;
	gHowToKeyboard[0].x = SCREEN_WIDTH / 2 - keysize / 2;

	gHowToKeyboard[1].y = gHowToKeyboard[0].y + keysize + 5;
	gHowToKeyboard[1].w = keysize;
	gHowToKeyboard[1].h = keysize;
	gHowToKeyboard[1].x = SCREEN_WIDTH / 2 + keysize / 2 + 5;

	gHowToKeyboard[2].y = gHowToKeyboard[0].y + keysize + 5;
	gHowToKeyboard[2].w = keysize;
	gHowToKeyboard[2].h = keysize;
	gHowToKeyboard[2].x = SCREEN_WIDTH / 2 - keysize / 2;

	gHowToKeyboard[3].y = gHowToKeyboard[0].y + keysize + 5;
	gHowToKeyboard[3].w = keysize;
	gHowToKeyboard[3].h = keysize;
	gHowToKeyboard[3].x = SCREEN_WIDTH / 2 - 3 * keysize / 2 - 5;

	gHowToKeyboard[4].y = grHowToPlayText[0].y + grHowToPlayText[0].h + 10;
	gHowToKeyboard[4].w = keysize;
	gHowToKeyboard[4].h = keysize;
	gHowToKeyboard[4].x = SCREEN_WIDTH / 2 - keysize / 2 + 5 * keysize / 2 + 50;

	gHowToKeyboard[5].y = gHowToKeyboard[0].y + keysize + 5;
	gHowToKeyboard[5].w = keysize;
	gHowToKeyboard[5].h = keysize;
	gHowToKeyboard[5].x = SCREEN_WIDTH / 2 + keysize / 2 + 5 + 5 * keysize / 2 + 50;

	gHowToKeyboard[6].y = gHowToKeyboard[0].y + keysize + 5;
	gHowToKeyboard[6].w = keysize;
	gHowToKeyboard[6].h = keysize;
	gHowToKeyboard[6].x = SCREEN_WIDTH / 2 - keysize / 2 + 5 * keysize / 2 + 50;

	gHowToKeyboard[7].y = gHowToKeyboard[0].y + keysize + 5;
	gHowToKeyboard[7].w = keysize;
	gHowToKeyboard[7].h = keysize;
	gHowToKeyboard[7].x = SCREEN_WIDTH / 2 - 3 * keysize / 2 - 5 + 5 * keysize / 2 + 50;

	int midboxcenter = (400 - (grHowToPlayText[3].y + grHowToPlayText[3].h)) / 2 + grHowToPlayText[3].y + grHowToPlayText[3].h;

	gHowToKeyboard[8].y = midboxcenter - keysize / 2;
	gHowToKeyboard[8].w = keysize*2;
	gHowToKeyboard[8].h = keysize;
	gHowToKeyboard[8].x = SCREEN_WIDTH / 8 * 7 - gHowToKeyboard[8].w / 2;

	for (i = 0; i < 9; i++)
	{
		grHowToPlayKeyText[i].x = gHowToKeyboard[i].x + 8;
		grHowToPlayKeyText[i].y = gHowToKeyboard[i].y + 5;
		grHowToPlayKeyText[i].w = gHowToKeyboard[i].w - 16;
		grHowToPlayKeyText[i].h = gHowToKeyboard[i].h - 10;
	}

	gHowToBoostText.w = gBoost_rect.w;
	gHowToBoostText.h = gBoost_rect.h;
	gHowToBoostText.x = SCREEN_WIDTH / 2 + 15;
	gHowToBoostText.y = grHowToPlayText[3].y + grHowToPlayText[3].h + 10;

	gHowToBoostMeter[0].h = gHowToBoostText.h - 6;
	gHowToBoostMeter[0].w = 138;
	gHowToBoostMeter[0].x = gHowToBoostText.x + gHowToBoostText.w + 5;
	gHowToBoostMeter[0].y = gHowToBoostText.y + 3;

	gHowToBoostMeter[1].h = gHowToBoostText.h - 6;
	gHowToBoostMeter[1].x = gHowToBoostText.x + gHowToBoostText.w + 5;
	gHowToBoostMeter[1].y = gHowToBoostText.y + 3;
	gHowToBoostMeter[1].w = 138;

	for (i = 0; i < 5; i++)
	{
		gHowToDemoRects[i].w = MENU_BOX_SIZE;
		gHowToDemoRects[i].h = MENU_BOX_SIZE;
	}

	gHowToDemoRects[0].x = SCREEN_WIDTH / 4 - 61;
	gHowToDemoRects[0].y = midboxcenter - gHowToDemoRects[0].h / 2;
	gHowToDemoRects[1].x = SCREEN_WIDTH / 4 + 61;
	gHowToDemoRects[1].y = midboxcenter - gHowToDemoRects[1].h / 2;

	gHowToDemoRects[2].x = gHowToBoostText.x;
	gHowToDemoRects[2].y = midboxcenter + 15;

	gHowToVictory.w = SCREEN_WIDTH / 2 - 20;
	gHowToVictory.y = grHowToPlayText[5].y + grHowToPlayText[5].h + 10;
	gHowToVictory.h = 400 - 10 - gHowToVictory.y;
	gHowToVictory.x = 10;
	
	gHowToPlayArea[0].y = grHowToPlayText[4].y + grHowToPlayText[4].h + 5;
	gHowToPlayArea[0].h = gBack_rect.y - 15 - gHowToPlayArea[0].y;
	gHowToPlayArea[0].w = gHowToPlayArea[0].h / 3 * 4;
	gHowToPlayArea[0].x = SCREEN_WIDTH / 2 - gHowToPlayArea[0].w / 2;

	gHowToPlayArea[1].x = gHowToPlayArea[0].x + 3;
	gHowToPlayArea[1].y = gHowToPlayArea[0].y + 3;
	gHowToPlayArea[1].w = gHowToPlayArea[0].w - 6;
	gHowToPlayArea[1].h = gHowToPlayArea[0].h - 6;

	gHowToDemoRects[3].x = gHowToPlayArea[1].x + 3;
	gHowToDemoRects[4].x = gHowToPlayArea[1].x + 1;
	gHowToDemoRects[3].y = gHowToPlayArea[1].y + gHowToPlayArea[1].h - 10 - 30;
	gHowToDemoRects[4].y = gHowToPlayArea[1].y + 17;

	grHowToHitPoints[0].y = gHowToPlayArea[0].y + gHowToPlayArea[0].h / 2 - grHowToHitPoints[0].h / 2;
	grHowToHitPoints[1].y = grHowToHitPoints[0].y;
	grHowToHitPoints[2].y = grHowToHitPoints[0].y;

	grHowToHitPoints[0].x = gHowToPlayArea[0].x - gHowToPlayArea[0].w - 7;
	grHowToHitPoints[1].x = grHowToHitPoints[0].x;
	grHowToHitPoints[2].x = grHowToHitPoints[0].x;

	grHowToGameOver.h = gHowToPlayArea[0].h * 1/2;
	float scale = (float)grHowToGameOver.h / (float)ggameover_rect.h * (float)ggameover_rect.w;
	grHowToGameOver.w = (int)scale;
	grHowToGameOver.x = SCREEN_WIDTH / 2 - grHowToGameOver.w / 2;
	grHowToGameOver.y = gHowToPlayArea[0].y + (gHowToPlayArea[0].h - grHowToGameOver.h) / 2;

	gHowToPlayFrame = 0;
	//end how to play menu

	//load high scores menu resources
	gHighScores_rect.x = SCREEN_WIDTH / 2 - gHighScores_rect.w / 2;
	gHighScores_rect.y = 10;

	gNameTitle_rect.x = 75;
	gNameTitle_rect.y = gHighScores_rect.y + gHighScores_rect.h + 5;
	
	gLevelTitle_rect.x = gNameTitle_rect.x + 6 * gNameTitle_rect.w;
	gLevelTitle_rect.y = gNameTitle_rect.y;

	gTimeTitle_rect.x = SCREEN_WIDTH - gTimeTitle_rect.w - 10;
	gTimeTitle_rect.y = gNameTitle_rect.y;
	
	gRanking_rect[0].x = 25;
	gRanking_rect[0].y = gNameTitle_rect.y + gNameTitle_rect.h + 5;

	for (i = 1; i < 10; i++)
	{		
		gRanking_rect[i].x = gRanking_rect[0].x;
		gRanking_rect[i].y = gRanking_rect[i - 1].y + gRanking_rect[i - 1].h - 8;
	}
	//end high scores menu

	//set default menu options
	gMenuOptions = STARTGAME;
	gCurrentMenu = MAIN_MENU;
	gResolutionOptions = r800x600;
	SDL_StopTextInput();
	//load the save values and set
	openSaveData();
	updateVolumes();
	if (gResolutionOptions != r800x600) //resolution changed
		gUpdateResolution = true;
}

void freeShellResources(void)
{
	int i, j;

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

	for (i = 0; i < MAX_RESOLUTIONS; i++)
	{
		if (gtResolution[i] != NULL)
			SDL_DestroyTexture(gtResolution[i]);

		if (gtResolutionSel[i] != NULL)
			SDL_DestroyTexture(gtResolutionSel[i]);
	}
	
	for (i = 0; i < NUM_HOW_TO_PLAY; i++)
		if (gtHowToPlayText[i] != NULL)
			SDL_DestroyTexture(gtHowToPlayText[i]);	

	for (i = 0; i < 6; i++)
		for (j = 0; j < 2; j++)
			if (gtHowToPlayKeyText[i][j] != NULL)
				SDL_DestroyTexture(gtHowToPlayKeyText[i][j]);

	for (i = 0; i < 3; i++)
		if(gtHowToHitPoints[i] != NULL)
			SDL_DestroyTexture(gtHowToHitPoints[i]);
}

void openHighScoresScreen(SDL_Renderer *r)
{
	int i, m, s;
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
				gtHSPlayerNames[i] = makeTextTexture(r, MENU_LARGE, "_", TEXT_COLOR, BG_COLOR, BLENDED, &gHSPlayerNames_rect[i]);
			else
				gtHSPlayerNames[i] = makeTextTexture(r, MENU_LARGE, gHighScores[i].Name, TEXT_COLOR, BG_COLOR, BLENDED, &gHSPlayerNames_rect[i]);
			gHSPlayerNames_rect[i].x = gNameTitle_rect.x;
			gHSPlayerNames_rect[i].y = gRanking_rect[i].y;

			sprintf_s(buffer, sizeof(buffer), "%d", gHighScores[i].level);
			gtHSLevels[i] = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gHSLevels_rect[i]);
			gHSLevels_rect[i].x = gLevelTitle_rect.x + gLevelTitle_rect.w - gHSLevels_rect[i].w;
			gHSLevels_rect[i].y = gRanking_rect[i].y;

			m = gHighScores[i].time / 60;
			s = gHighScores[i].time - m * 60;
			sprintf_s(buffer, sizeof(buffer), "%02d:%02d", m, s);
			gtHSTimes[i] = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gHSTimes_rect[i]);
			
			gHSTimes_rect[i].x = gTimeTitle_rect.x + gTimeTitle_rect.w - gHSTimes_rect[i].w;
			gHSTimes_rect[i].y = gRanking_rect[i].y;
		}

	}
}

bool shellKeyboard(SDL_Event *e, SDL_Renderer *r)
{
	char nbuffer[MAX_NAME+20];
	bool quit = false;
	switch (gCurrentMenu)
	{
	case MAIN_MENU:
		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gMenuOptions > 0)
				gMenuOptions--;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gMenuOptions < numMenuOptions - 1)
				gMenuOptions++;
			break;
		case SDLK_RETURN:
		case SDLK_SPACE:
			switch (gMenuOptions)
			{
			case STARTGAME:
				Mix_HaltMusic();
				MoveToGame(r);				
				gMenuOptions = STARTGAME;
				gCurrentMenu = MAIN_MENU;
				break;
			case HOWTOPLAY:
				Mix_PlayChannel(-1, gClickSound, 0);
				gCurrentMenu = HOWTOPLAY_MENU;
				gHowToPlayFrame = 0;
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
		if (e->key.keysym.sym == SDLK_RETURN || e->key.keysym.sym == SDLK_SPACE)
		{
			Mix_PlayChannel(-1, gClickSound, 0);
			gCurrentMenu = MAIN_MENU;
			gHowToPlayFrame = 0;
			gHowToDemoRects[2].x = gHowToBoostText.x;
			gHowToBoostMeter[1].w = 138;
			gHowToDemoRects[3].x = gHowToPlayArea[1].x + 3;
			gHowToDemoRects[4].x = gHowToPlayArea[1].x + 1;
			gHowToDemoRects[3].y = gHowToPlayArea[1].y + gHowToPlayArea[1].h - 10 - 30;
			gHowToDemoRects[4].y = gHowToPlayArea[1].y + 17;
		}
		break;
	case OPTIONS_MENU:
		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gOptionsMenuOptions > 0)
				gOptionsMenuOptions--;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			Mix_PlayChannel(-1, gClickSound, 0);
			if (gOptionsMenuOptions < BACK_OPTIONS)
				gOptionsMenuOptions++;
			break;
		case SDLK_RETURN:
		case SDLK_SPACE:
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
		case SDLK_a:
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
			case RESOLUTION:
				if (gResolutionOptions == 0)
					gResolutionOptions = MAX_RESOLUTIONS - 1;
				else
					gResolutionOptions--;
				gUpdateResolution = true;
				break;				
			}
			break;
		case SDLK_RIGHT:
		case SDLK_d:
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
			case RESOLUTION:
				if (gResolutionOptions == MAX_RESOLUTIONS-1)
					gResolutionOptions = 0;
				else
					gResolutionOptions++;
				gUpdateResolution = true;
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
			gtHSPlayerNames[gHSRank] = makeTextTexture(r, MENU_LARGE, nbuffer, TEXT_COLOR, BG_COLOR, BLENDED, &gHSPlayerNames_rect[gHSRank]);
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
	//int w, h;

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
			gtHSPlayerNames[gHSRank] = makeTextTexture(r, MENU_LARGE, nbuffer, TEXT_COLOR, BG_COLOR, BLENDED, &gHSPlayerNames_rect[gHSRank]);
		}
		else
		{
			gtHSPlayerNames[gHSRank] = makeTextTexture(r, MENU_LARGE, gNameInput, TEXT_COLOR, BG_COLOR, BLENDED, &gHSPlayerNames_rect[gHSRank]);
		}
	}
}

void shellLogic(SDL_Renderer *r)
{
	int i;
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
			gtMasterVolume = makeTextTexture(r, MENU_LARGE, buffer, Select_Color, BG_COLOR, BLENDED, &gMasterVolume_rect);
		else
			gtMasterVolume = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gMasterVolume_rect);
	
		if (gtMusicVolume != NULL)
			SDL_DestroyTexture(gtMusicVolume);
		sprintf_s(buffer, sizeof(buffer), "Music Volume: %d", gMusicVolume);
		if (gOptionsMenuOptions == MUSICVOLUME)
			gtMusicVolume = makeTextTexture(r, MENU_LARGE, buffer, Select_Color, BG_COLOR, BLENDED, &gMusicVolume_rect);
		else
			gtMusicVolume = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gMusicVolume_rect);

		if (gtSoundVolume != NULL)
			SDL_DestroyTexture(gtSoundVolume);
		sprintf_s(buffer, sizeof(buffer), "Sound Volume: %d", gSoundVolume);
		if (gOptionsMenuOptions == SOUNDVOLUME)
			gtSoundVolume = makeTextTexture(r, MENU_LARGE, buffer, Select_Color, BG_COLOR, BLENDED, &gSoundVolume_rect);
		else
			gtSoundVolume = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gSoundVolume_rect);

		if (gDrawFPS)
			sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): On");
		else
			sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): Off");

		if (gtFPSOnOff != NULL)
			SDL_DestroyTexture(gtFPSOnOff);
		if (gOptionsMenuOptions == FPSONOFF)
			gtFPSOnOff = makeTextTexture(r, MENU_LARGE, buffer, Select_Color, BG_COLOR, BLENDED, &gFPSOnOff_rect);
		else
			gtFPSOnOff = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gFPSOnOff_rect);
	}
	
}

void drawShell(SDL_Renderer *r)
{
	int i, startx, starty, x, y;
	SDL_Rect tempbox;
	int speed = 1;
	int movespeed;

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
		for (i = 0; i < NUM_HOW_TO_PLAY; i++)
			SDL_RenderCopy(r, gtHowToPlayText[i], NULL, &grHowToPlayText[i]); //all text
		SDL_SetRenderDrawColor(r,255,255,255,SDL_ALPHA_OPAQUE);
		for (i = 0; i < 3; i++)
		{
			SDL_RenderDrawLine(r, 0, gHowToPlay_rect.y + gHowToPlay_rect.h + 2 + i, 800, gHowToPlay_rect.y + gHowToPlay_rect.h + 2 + i);
			SDL_RenderDrawLine(r, 0, 240 + i, 800, 240 + i);
			SDL_RenderDrawLine(r, SCREEN_WIDTH / 2 - 1 + i, 240, SCREEN_WIDTH / 2 - 1 + i, 400);
			SDL_RenderDrawLine(r, 0, 402 + i, 800, 402 + i);
			SDL_RenderDrawLine(r, 0, gBack_rect.y - 8, 800, gBack_rect.y - 8);
		}
		int quadrant = gHowToPlayFrame / 60;
		for (i = 0; i < 8; i++)
		{
			if (i == quadrant || (i > 3 && i - 4 == quadrant))
			{
				SDL_RenderFillRect(r, &gHowToKeyboard[i]);
				if (i > 3)
					SDL_RenderCopyEx(r, gtHowToPlayKeyText[4][1], NULL, &grHowToPlayKeyText[i], 90 * (i - 4), NULL, SDL_FLIP_NONE);
				else
					SDL_RenderCopy(r, gtHowToPlayKeyText[i][1], NULL, &grHowToPlayKeyText[i]);
			}
			else
			{
				SDL_RenderDrawRect(r, &gHowToKeyboard[i]);
				if (i > 3)
					SDL_RenderCopyEx(r, gtHowToPlayKeyText[4][0], NULL, &grHowToPlayKeyText[i], 90 * (i-4), NULL, SDL_FLIP_NONE);
				else
					SDL_RenderCopy(r, gtHowToPlayKeyText[i][0], NULL, &grHowToPlayKeyText[i]);
			}
		}
		
		//draw the player movement sequence
		startx = SCREEN_WIDTH / 2 - 3 * gHowToKeyboard[0].w/2 - 110;
		starty = 215; //starting position
		x = startx;
		y = starty;

		y = y - speed * gHowToPlayFrame;
		if (y < starty - 60)
			y = starty - 60;

		x = (gHowToPlayFrame > 60) ? x + speed * (gHowToPlayFrame - 60) : x;
		if (x > startx + 60)
			x = startx + 60;

		y = (gHowToPlayFrame > 120) ? y + speed * (gHowToPlayFrame - 120) : y;
		if (gHowToPlayFrame > 180)
			y = starty;

		x = (gHowToPlayFrame > 180) ? x - speed * (gHowToPlayFrame - 180) : x;

		tempbox.w = 10;
		tempbox.h = 10;
		tempbox.x = x;
		tempbox.y = y;
		DrawBox(r, &tempbox, WHITE);

		//goal section
		if (gHowToPlayFrame < 120)
		{
			tempbox.x = gHowToDemoRects[0].x + gHowToPlayFrame;
			tempbox.y = gHowToDemoRects[0].y;
			DrawBox(r, &tempbox, WHITE);
			DrawBox(r, &gHowToDemoRects[1], BLUE);
		}
		else
			SDL_RenderCopy(r, gtvictory, NULL, &gHowToVictory);

		//boost section
		SDL_SetRenderDrawColor(r, 255, 255, 255, SDL_ALPHA_OPAQUE);		
		if (quadrant == 0 || quadrant == 2)
		{			
			SDL_RenderFillRect(r, &gHowToKeyboard[8]);
			SDL_RenderCopy(r, gtHowToPlayKeyText[5][1], NULL, &grHowToPlayKeyText[8]);
		}
		else
		{
			SDL_RenderDrawRect(r, &gHowToKeyboard[8]);
			SDL_RenderCopy(r, gtHowToPlayKeyText[5][0], NULL, &grHowToPlayKeyText[8]);
		}
		switch (quadrant)
		{
		case 0:
			movespeed = 2;
			gHowToBoostMeter[1].w -= 1;
			break;
		case 1:
			movespeed = 1;
			gHowToBoostMeter[1].w += 1;
			break;
		case 2:
			movespeed = -2;
			gHowToBoostMeter[1].w -= 1;
			break;
		case 3:
			movespeed = -1;
			gHowToBoostMeter[1].w += 1;
			break;
		default:
			movespeed = 0;
			break;
		}
		gHowToDemoRects[2].x += movespeed;

		SDL_RenderCopy(r, gtBoost, NULL, &gHowToBoostText);
		SDL_SetRenderDrawColor(r, 34, 139, 34, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(r, &gHowToBoostMeter[0]);
		DrawBox(r, &gHowToBoostMeter[1], GREEN);
		DrawBox(r, &gHowToDemoRects[2], WHITE);
		
		//enemy section
		if (gHowToPlayFrame < 180)
		{
			if (gHowToPlayFrame >= 62 && gHowToPlayFrame < 67)
				DrawBox(r, &gHowToPlayArea[0], RED);
			else
				DrawBox(r, &gHowToPlayArea[0], WHITE);
			DrawBox(r, &gHowToPlayArea[1], BLACK);
		}
		else
			SDL_RenderCopy(r, gtgameover, NULL, &grHowToGameOver);

		if (gHowToPlayFrame < 62)
		{
			gHowToDemoRects[3].x += gHowToPlayFrame % 2;
			gHowToDemoRects[3].y += gHowToPlayFrame % 2;
		}
		else
		{
			gHowToDemoRects[3].x += gHowToPlayFrame % 2;
			gHowToDemoRects[3].y -= gHowToPlayFrame % 2;
		}
		gHowToDemoRects[4].x += gHowToPlayFrame % 2;

		if (gHowToPlayFrame < 180)
		{
			if (gHowToPlayFrame >= 62 && gHowToPlayFrame < 67)
				DrawBox(r, &gHowToDemoRects[3], RED);
			else if (gHowToPlayFrame >= 67 && gHowToPlayFrame < 120)
			{
				if (gHowToPlayFrame % 10 < 5)
					DrawBox(r, &gHowToDemoRects[3], WHITE);
			}
			else
				DrawBox(r, &gHowToDemoRects[3], WHITE);
			DrawBox(r, &gHowToDemoRects[4], ORANGE);
		}		

		if (gHowToPlayFrame < 62)
			SDL_RenderCopy(r, gtHowToHitPoints[0], NULL, &grHowToHitPoints[0]);
		else if (gHowToPlayFrame < 180)
			SDL_RenderCopy(r, gtHowToHitPoints[1], NULL, &grHowToHitPoints[1]);
		else
			SDL_RenderCopy(r, gtHowToHitPoints[2], NULL, &grHowToHitPoints[2]);
		

		//draw back button
		tempbox.w = 10;
		tempbox.h = 10;
		tempbox.y = gBack_rect.y + gBack_rect.h / 2 - tempbox.h / 2 + 1;
		tempbox.x = gBack_rect.x - tempbox.w - 10;
		DrawBox(r, &tempbox, WHITE);
		tempbox.x = gBack_rect.x + gBack_rect.w + 10;
		DrawBox(r, &tempbox, WHITE);
		SDL_RenderCopy(r, gtBackSel, NULL, &gBack_rect);
		gHowToPlayFrame++; //increase the how to play frame counter
		if (gHowToPlayFrame > 240)
		{
			gHowToPlayFrame = 0; //if 240 frames (0-119) have passed reset to zero
			gHowToDemoRects[3].x = gHowToPlayArea[1].x + 3;
			gHowToDemoRects[4].x = gHowToPlayArea[1].x + 1;
			gHowToDemoRects[3].y = gHowToPlayArea[1].y + gHowToPlayArea[1].h - 10 - 30;
			gHowToDemoRects[4].y = gHowToPlayArea[1].y + 17;
		}
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
		if (gOptionsMenuOptions == RESOLUTION)
			SDL_RenderCopy(r, gtResolutionSel[gResolutionOptions], NULL, &gResolution_rect[gResolutionOptions]);
		else
			SDL_RenderCopy(r, gtResolution[gResolutionOptions], NULL, &gResolution_rect[gResolutionOptions]);
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
		case RESOLUTION:
			tempbox.y = gResolution_rect[gResolutionOptions].y + gResolution_rect[gResolutionOptions].h / 2 - tempbox.h / 2 + 1;
			tempbox.x = gResolution_rect[gResolutionOptions].x - tempbox.w - 10;
			DrawBox(r, &tempbox, WHITE);
			tempbox.x = gResolution_rect[gResolutionOptions].x + gResolution_rect[gResolutionOptions].w + 10;
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
	int master, music, sound, resolution;
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
#ifdef _DEBUG_BUILD_
		printf("Save data loaded.\n");
#endif

		master = -1; music = -1; sound = -1;
		fscanf(datafile, "%d, %d, %d, %d", &master, &music, &sound, &resolution);
		fgets(buffer, 25, datafile); //grab the newline char to clean up 
#ifdef _DEBUG_BUILD_
		printf("Found: %d, %d, %d, %d\n", master, music, sound, resolution);
#endif
		if (master > MAX_VOLUME || master < 0 ||
			music > MAX_VOLUME || music < 0 ||
			sound > MAX_VOLUME || sound < 0 ||
			resolution >= MAX_RESOLUTIONS || resolution < 0)
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
#ifdef _DEBUG_BUILD_
				printf("%d: Name: %s, Level: %d, Time: %d\n", i + 1, name, level, time);
#endif
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
			gResolutionOptions = resolution;
		}
	}

	if (datafile != NULL)
		fclose(datafile);
}

void writeGameDataFile(void)
{
	int i;
	int resolution = (int)gResolutionOptions;
	FILE *datafile;

	datafile = fopen(SAVE_DATA, "w");
	if (datafile == NULL)
		printf("Cannot write to data file: %s\n", SAVE_DATA);
	else
	{
		fprintf(datafile, "%d, %d, %d, %d\n", gMasterVolume, gMusicVolume, gSoundVolume, resolution);

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

bool loadTextResources(SDL_Renderer *r)
{
	SDL_Color Select_Color = { 0, 255, 255 };
	int i, j;
	char buffer[100];

	if (gtTitle != NULL)
		SDL_DestroyTexture(gtTitle);
	gtTitle = makeTextTexture(r, STANDARD_LARGE, GAME_NAME, TEXT_COLOR, BG_COLOR, BLENDED, &gTitle_rect);

	for (i = 0; i < 5; i++)
	{
		if (gtMenuOptions[i] != NULL)
			SDL_DestroyTexture(gtMenuOptions[i]);
		if (gtMenuOptionsSel[i] != NULL)
			SDL_DestroyTexture(gtMenuOptionsSel[i]);
	}
	gtMenuOptions[0] = makeTextTexture(r, MENU_LARGE, "Start Game", TEXT_COLOR, BG_COLOR, BLENDED, &gMenuOptions_rect[0]);
	gtMenuOptions[1] = makeTextTexture(r, MENU_LARGE, "How to Play", TEXT_COLOR, BG_COLOR, BLENDED, &gMenuOptions_rect[1]);
	gtMenuOptions[2] = makeTextTexture(r, MENU_LARGE, "High Scores", TEXT_COLOR, BG_COLOR, BLENDED, &gMenuOptions_rect[2]);
	gtMenuOptions[3] = makeTextTexture(r, MENU_LARGE, "Options", TEXT_COLOR, BG_COLOR, BLENDED, &gMenuOptions_rect[3]);
	gtMenuOptions[4] = makeTextTexture(r, MENU_LARGE, "Exit", TEXT_COLOR, BG_COLOR, BLENDED, &gMenuOptions_rect[4]);

	gtMenuOptionsSel[0] = makeTextTexture(r, MENU_LARGE, "Start Game", Select_Color, BG_COLOR, BLENDED, &gMenuOptions_rect[0]);
	gtMenuOptionsSel[1] = makeTextTexture(r, MENU_LARGE, "How to Play", Select_Color, BG_COLOR, BLENDED, &gMenuOptions_rect[1]);
	gtMenuOptionsSel[2] = makeTextTexture(r, MENU_LARGE, "High Scores", Select_Color, BG_COLOR, BLENDED, &gMenuOptions_rect[2]);
	gtMenuOptionsSel[3] = makeTextTexture(r, MENU_LARGE, "Options", Select_Color, BG_COLOR, BLENDED, &gMenuOptions_rect[3]);
	gtMenuOptionsSel[4] = makeTextTexture(r, MENU_LARGE, "Exit", Select_Color, BG_COLOR, BLENDED, &gMenuOptions_rect[4]);

	for (i = 0; i < 3; i++)
	{
		if (gtVersionText[i] != NULL)
			SDL_DestroyTexture(gtVersionText[i]);
	}

	sprintf_s(buffer, sizeof(buffer), "Version %d.%03d", BUILD_NUMBER / 1000, BUILD_NUMBER % 1000);
	gtVersionText[0] = makeTextTexture(r, MENU_SMALL, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gVersion_rect[0]);
	gtVersionText[1] = makeTextTexture(r, MENU_SMALL,
		"Game Music Copyright 2017 by Brian Hicks", TEXT_COLOR, BG_COLOR, BLENDED, &gVersion_rect[1]);
	gtVersionText[2] = makeTextTexture(r, MENU_SMALL,
		"Game Code Copyright 2017-2021 under GNU GPLv3 by Matthew K. Daddysman", TEXT_COLOR, BG_COLOR, BLENDED, &gVersion_rect[2]);

	if (gtBack != NULL)
		SDL_DestroyTexture(gtBack);
	if (gtBackSel != NULL)
		SDL_DestroyTexture(gtBackSel);
	gtBack = makeTextTexture(r, MENU_LARGE, "BACK", TEXT_COLOR, BG_COLOR, BLENDED, &gBack_rect);
	gtBackSel = makeTextTexture(r, MENU_LARGE, "BACK", Select_Color, BG_COLOR, BLENDED, &gBack_rect);

	if (gtOptions != NULL)
		SDL_DestroyTexture(gtOptions);
	gtOptions = makeTextTexture(r, STANDARD_LARGE, "Options", TEXT_COLOR, BG_COLOR, BLENDED, &gOptions_rect);
	
	if (gtMasterVolume != NULL)
		SDL_DestroyTexture(gtMasterVolume);
	sprintf_s(buffer, sizeof(buffer), "Master Volume: %d", gMasterVolume);
	gtMasterVolume = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gMasterVolume_rect);
	
	if (gtMusicVolume != NULL)
		SDL_DestroyTexture(gtMusicVolume);
	sprintf_s(buffer, sizeof(buffer), "Music Volume: %d", gMusicVolume);
	gtMusicVolume = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gMusicVolume_rect);
	
	if (gtSoundVolume != NULL)
		SDL_DestroyTexture(gtSoundVolume);
	sprintf_s(buffer, sizeof(buffer), "Sound Volume: %d", gSoundVolume);
	gtSoundVolume = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gSoundVolume_rect);
	
	if (gtFPSOnOff != NULL)
		SDL_DestroyTexture(gtFPSOnOff);
	if (gDrawFPS)
		sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): On");
	else
		sprintf_s(buffer, sizeof(buffer), "Display Frames per Second (FPS): Off");
	gtFPSOnOff = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gFPSOnOff_rect);

	for (i = 0; i < 3; i++)
	{
		if (gtResolution[i] != NULL)
			SDL_DestroyTexture(gtResolution[i]);
		if (gtResolutionSel[i] != NULL)
			SDL_DestroyTexture(gtResolutionSel[i]);
	}
	gtResolution[0] = makeTextTexture(r, MENU_LARGE, "Resolution: 800 x 600", TEXT_COLOR, BG_COLOR, BLENDED, &gResolution_rect[0]);
	gtResolutionSel[0] = makeTextTexture(r, MENU_LARGE, "Resolution: 800 x 600", Select_Color, BG_COLOR, BLENDED, &gResolution_rect[0]);
	gtResolution[1] = makeTextTexture(r, MENU_LARGE, "Resolution: 1024 x 768", TEXT_COLOR, BG_COLOR, BLENDED, &gResolution_rect[1]);
	gtResolutionSel[1] = makeTextTexture(r, MENU_LARGE, "Resolution: 1024 x 768", Select_Color, BG_COLOR, BLENDED, &gResolution_rect[1]);
	gtResolution[2] = makeTextTexture(r, MENU_LARGE, "Resolution: 1280 x 960", TEXT_COLOR, BG_COLOR, BLENDED, &gResolution_rect[2]);
	gtResolutionSel[2] = makeTextTexture(r, MENU_LARGE, "Resolution: 1280 x 960", Select_Color, BG_COLOR, BLENDED, &gResolution_rect[2]);

	if (gtHowToPlay != NULL)
		SDL_DestroyTexture(gtHowToPlay);
	gtHowToPlay = makeTextTexture(r, STANDARD_LARGE, "How to Play", TEXT_COLOR, BG_COLOR, BLENDED, &gHowToPlay_rect);
	
	if (gtHighScores != NULL)
		SDL_DestroyTexture(gtHighScores);
	gtHighScores = makeTextTexture(r, STANDARD_LARGE, "High Scores", TEXT_COLOR, BG_COLOR, BLENDED, &gHighScores_rect);
	
	if (gtNameTitle != NULL)
		SDL_DestroyTexture(gtNameTitle);
	gtNameTitle = makeTextTexture(r, MENU_LARGE, "Player", TEXT_COLOR, BG_COLOR, BLENDED, &gNameTitle_rect);
	
	if (gtLevelTitle != NULL)
		SDL_DestroyTexture(gtLevelTitle);
	gtLevelTitle = makeTextTexture(r, MENU_LARGE, "Level", TEXT_COLOR, BG_COLOR, BLENDED, &gLevelTitle_rect);
	
	if (gtTimeTitle != NULL)
		SDL_DestroyTexture(gtTimeTitle);
	gtTimeTitle = makeTextTexture(r, MENU_LARGE, "Time", TEXT_COLOR, BG_COLOR, BLENDED, &gTimeTitle_rect);
	
	for (i = 0; i < 10; i++)
	{
		if (gtRanking[i] != NULL)
			SDL_DestroyTexture(gtRanking[i]);
		sprintf_s(buffer, sizeof(buffer), "%d", i + 1);
		gtRanking[i] = makeTextTexture(r, MENU_LARGE, buffer, TEXT_COLOR, BG_COLOR, BLENDED, &gRanking_rect[i]);
	}

	for (i = 0; i < NUM_HOW_TO_PLAY; i++)
	{
		if (gtHowToPlayText[i] != NULL)
			SDL_DestroyTexture(gtHowToPlayText[i]);
		gtHowToPlayText[i] = NULL;
	}

	gtHowToPlayText[0] = makeTextTexture(r, MENU_SMALL, "The player is a white box. Use WASD or arrow keys to move the player.", TEXT_COLOR, BG_COLOR, BLENDED, &grHowToPlayText[0]);
	gtHowToPlayText[1] = makeTextTexture(r, MENU_SMALL, "The goal is to touch the blue box.", TEXT_COLOR, BG_COLOR, BLENDED, &grHowToPlayText[1]);
	gtHowToPlayText[2] = makeTextTexture(r, MENU_SMALL, "Hold CTRL to use the boost.", TEXT_COLOR, BG_COLOR, BLENDED, &grHowToPlayText[2]);
	gtHowToPlayText[3] = makeTextTexture(r, MENU_SMALL, "Using the boost depletes the boost supply.", TEXT_COLOR, BG_COLOR, BLENDED, &grHowToPlayText[3]);
	gtHowToPlayText[4] = makeTextTexture(r, MENU_SMALL, "Avoid orange and yellow boxes and the edge. Touching either removes a Hit Point.", TEXT_COLOR, BG_COLOR, BLENDED, &grHowToPlayText[4]);
	gtHowToPlayText[5] = makeTextTexture(r, MENU_SMALL, "The challenge increases with each level.", TEXT_COLOR, BG_COLOR, BLENDED, &grHowToPlayText[5]);

	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 2; j++)
		{
			if (gtHowToPlayKeyText[i][j] != NULL)
				SDL_DestroyTexture(gtHowToPlayKeyText[i][j]);
			gtHowToPlayKeyText[i][j] = NULL;
		}
	}

	gtHowToPlayKeyText[0][0] = makeTextTexture(r, MENU_LARGE, "W", TEXT_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[1][0] = makeTextTexture(r, MENU_LARGE, "D", TEXT_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[2][0] = makeTextTexture(r, MENU_LARGE, "S", TEXT_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[3][0] = makeTextTexture(r, MENU_LARGE, "A", TEXT_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[4][0] = makeTextTexture(r, MENU_LARGE, "^", TEXT_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[5][0] = makeTextTexture(r, MENU_LARGE, "CTRL", TEXT_COLOR, BG_COLOR, BLENDED, &gDummyRect);

	gtHowToPlayKeyText[0][1] = makeTextTexture(r, MENU_LARGE, "W", BG_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[1][1] = makeTextTexture(r, MENU_LARGE, "D", BG_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[2][1] = makeTextTexture(r, MENU_LARGE, "S", BG_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[3][1] = makeTextTexture(r, MENU_LARGE, "A", BG_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[4][1] = makeTextTexture(r, MENU_LARGE, "^", BG_COLOR, BG_COLOR, BLENDED, &gDummyRect);
	gtHowToPlayKeyText[5][1] = makeTextTexture(r, MENU_LARGE, "CTRL", BG_COLOR, BG_COLOR, BLENDED, &gDummyRect);

	gtHowToHitPoints[0] = makeTextTexture(r, STANDARD_SMALL, "Hit Points: 1", TEXT_COLOR, BG_COLOR, SHADED, &grHowToHitPoints[0]);
	gtHowToHitPoints[1] = makeTextTexture(r, STANDARD_SMALL, "Hit Points: 0", TEXT_COLOR, BG_COLOR, SHADED, &grHowToHitPoints[1]);
	gtHowToHitPoints[2] = makeTextTexture(r, STANDARD_SMALL, "Hit Points:  ", TEXT_COLOR, BG_COLOR, SHADED, &grHowToHitPoints[2]);

	return loadGameTextResources(r);
}

