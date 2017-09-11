/*
* Box Game - game.c
* Box Game to demo SDL2 and simple AI
* This file includes functions for running the game 
* Forked from SDL Learning repo on 9/3/2017
* By Matthew K. Daddysman
*/

#include "main.h"

int gLevel, gHitPoints, gNumAIs; //the current Level, Hit Points, and the speed at which the AI moves
float gBoostPool, gBoostDown, gBoostRecharge; //the Boost pool in percent. float for fraction and how fast it charges and recharges
int gLeftEdge, gRightEdge, gTopEdge, gBottomEdge; //define the edges of the playfield for bounds checking
unsigned long int gCurrentLevelTime, gPreviousLevelTime; //keep track of the times being played
unsigned short int gCountdownSecCount; //how many seconds have run off the countdown 
Uint32 gStartLevelTick;
Mix_Music *gMusic;
Mix_Chunk *gCountdownSound, *gHitSound, *gDeathSound, *gWinSound;;
SDL_Texture *gtLevelNumber, *gtHitPoints, *gtBoost, *gtTime;
SDL_Rect gLevel_rect, gHitPoints_rect, gBoost_rect, gBoostmeter_rect, gBoostdeplet_rect, gTime_rect, gBorder, gPlayarea;
SDL_Texture *gtmoreAI, *gthardGoal, *gtrecover, *gtfasterAI, *gtfasterrecharge, *gtyellowbox, *gtgameover, *gtvictory, *gtcontinue; //end game messages
SDL_Rect ggameover_rect, gvictory_rect, gcontinue_rect;
SDL_Rect gupgradeRects[5];  //end game message storage and rect 
SDL_Texture *gupgradeTextures[5];
SDL_Rect gcountdownRects[3];
SDL_Texture *gcountdownTextures[3];
short int gnumUpgrades;
struct Player gPlayerBox;
struct AIBox gGoalBox, gEnemyBox[MAX_AI_BOXES];
enum GameState gGameState;
bool gPaused; 
SDL_Texture *gtResume, *gtQuit, *gtResumeSel, *gtQuitSel;
SDL_Rect gResume_rect, gQuit_rect; 
int gPausedOption;

extern TTF_Font *gSmallFont, *gLargeFont, *gMenuFont; //extern fonts

void loadGameResources(SDL_Renderer *r)
{
	int i, w, h;
	SDL_Color Select_Color = { 0, 255, 255 };
	//initalize global variables 
	gMusic = NULL;
	gCountdownSound = NULL;
	gHitSound = NULL;
	gDeathSound = NULL;
	gWinSound = NULL;
	gtLevelNumber = NULL;
	gtHitPoints = NULL;
	gtBoost = NULL;
	gtTime = NULL;

	gtmoreAI = NULL;
	gthardGoal = NULL;
	gtrecover = NULL;
	gtfasterAI = NULL;
	gtfasterrecharge = NULL;
	gtyellowbox = NULL;

	gtgameover = NULL;
	gtvictory = NULL;
	gtcontinue = NULL;

	gtResume = NULL;
	gtResumeSel = NULL;
	gtQuit = NULL;
	gtQuitSel = NULL;	

	gPlayerBox.x = 200;
	gPlayerBox.y = 200;
	gPlayerBox.w = PLAYERBOX_WIDTH;
	gPlayerBox.h = PLAYERBOX_HEIGHT;
	gPlayerBox.color = WHITE;
	gPlayerBox.invernable = false;
	gPlayerBox.inv_draw = false;
	gPlayerBox.inv_count = 0;
	gPlayerBox.inv_flash_count = 0;

	gGoalBox.x = 200;
	gGoalBox.y = 200;
	gGoalBox.w = 10;
	gGoalBox.h = 10;
	gGoalBox.direction = NORTH;
	gGoalBox.rand_direction = 1;
	gGoalBox.color = BLUE;
	gGoalBox.speed = 1;
	gGoalBox.aitype = RANDOM;

	for (i = 0; i < MAX_AI_BOXES; i++)
	{
		gEnemyBox[i].x = 200;
		gEnemyBox[i].y = 200;
		gEnemyBox[i].w = 10;
		gEnemyBox[i].h = 10;
		gEnemyBox[i].direction = NORTH;
		gEnemyBox[i].rand_direction = i % 5;  //make each new box more apt to change direction within a speed group 
		if (i % 5 == 0 && i > 0)
		{
			gEnemyBox[i].color = YELLOW;
			gEnemyBox[i].speed = 1;
			gEnemyBox[i].aitype = SEEK;
		}
		else
		{
			gEnemyBox[i].color = ORANGE;
			gEnemyBox[i].speed = i / 5 + 1;
			gEnemyBox[i].aitype = RANDOM;
		}

	}


	gBorder.x = PLAYAREA_PADDING;
	gBorder.y = SCOREBAR_HEIGHT + PLAYAREA_PADDING;
	gBorder.w = SCREEN_WIDTH - 2 * PLAYAREA_PADDING;
	gBorder.h = SCREEN_HEIGHT - SCOREBAR_HEIGHT - 2 * PLAYAREA_PADDING;

	gPlayarea.x = PLAYAREA_PADDING + PLAYAREA_BORDER;
	gPlayarea.y = SCOREBAR_HEIGHT + PLAYAREA_PADDING + PLAYAREA_BORDER;
	gPlayarea.w = SCREEN_WIDTH - 2 * PLAYAREA_PADDING - 2 * PLAYAREA_BORDER;
	gPlayarea.h = SCREEN_HEIGHT - SCOREBAR_HEIGHT - 2 * PLAYAREA_PADDING - 2 * PLAYAREA_BORDER;

	gLeftEdge = gPlayarea.x;
	gTopEdge = gPlayarea.y;
	gRightEdge = gPlayarea.x + gPlayarea.w - PLAYERBOX_WIDTH;
	gBottomEdge = gPlayarea.y + gPlayarea.h - PLAYERBOX_HEIGHT;

	gGameState = NOGAME;
	//end initalize global variables 

	//load global resources
	gMusic = Mix_LoadMUS(GAME_MUSIC);
	gCountdownSound = Mix_LoadWAV(COUNTDOWN_SOUND);
	gHitSound = Mix_LoadWAV(WALL_SOUND);
	gDeathSound = Mix_LoadWAV(DEATH_SOUND);
	gWinSound = Mix_LoadWAV(WIN_SOUND);

	gtBoost = makeTextTexture(r, gSmallFont, "Boost:", TEXT_COLOR, BG_COLOR, SHADED);
	SDL_QueryTexture(gtBoost, NULL, NULL, &w, &h);
	gBoost_rect.x = gHitPoints_rect.x + gHitPoints_rect.w + 20;
	gBoost_rect.y = PLAYAREA_PADDING;
	gBoost_rect.w = w;
	gBoost_rect.h = h;

	gtgameover = makeTextTexture(r, gLargeFont, "GAME OVER", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtgameover, NULL, NULL, &w, &h);
	ggameover_rect.w = w;
	ggameover_rect.h = h;
	ggameover_rect.x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
	ggameover_rect.y = gPlayarea.y + gPlayarea.h / 2 - h / 2;

	gtvictory = makeTextTexture(r, gLargeFont, "Level Complete!", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtvictory, NULL, NULL, &w, &h);
	gvictory_rect.w = w;
	gvictory_rect.h = h;
	gvictory_rect.x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
	gvictory_rect.y = gPlayarea.y + gPlayarea.h / 2 - h / 2;

	gtcontinue = makeTextTexture(r, gSmallFont, "Press ENTER to continue", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtcontinue, NULL, NULL, &w, &h);
	gcontinue_rect.w = w;
	gcontinue_rect.h = h;
	gcontinue_rect.x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
	gcontinue_rect.y = gPlayarea.y + gPlayarea.h / 2 - h / 2;


	gtmoreAI = makeTextTexture(r, gSmallFont, "You will now face more opposition.", TEXT_COLOR, BG_COLOR, BLENDED);
	gtrecover = makeTextTexture(r, gSmallFont, "Good Work! You gain 1 hit point and a longer boost!", TEXT_COLOR, BG_COLOR, BLENDED);
	gthardGoal = makeTextTexture(r, gSmallFont, "Your goal will now behave more erratically.", TEXT_COLOR, BG_COLOR, BLENDED);
	gtfasterAI = makeTextTexture(r, gSmallFont, "Some of your opposition will now move faster.", TEXT_COLOR, BG_COLOR, BLENDED);
	gtfasterrecharge = makeTextTexture(r, gSmallFont, "Your boost will also recharge faster.", TEXT_COLOR, BG_COLOR, BLENDED);
	gtyellowbox = makeTextTexture(r, gSmallFont, "A new challenge! The yellow box will home in on your position.", TEXT_COLOR, BG_COLOR, BLENDED);

	gupgradeTextures[0] = gtmoreAI; gupgradeTextures[1] = gtfasterAI; gupgradeTextures[2] = gthardGoal; //avoid NULL pointers 
	gupgradeTextures[3] = gtrecover; gupgradeTextures[4] = gtfasterrecharge;

	gcountdownTextures[0] = makeTextTexture(r, gLargeFont, "3", TEXT_COLOR, BG_COLOR, BLENDED);
	gcountdownTextures[1] = makeTextTexture(r, gLargeFont, "2", TEXT_COLOR, BG_COLOR, BLENDED);
	gcountdownTextures[2] = makeTextTexture(r, gLargeFont, "1", TEXT_COLOR, BG_COLOR, BLENDED);
	for (i = 0; i < 3; i++)
	{
		SDL_QueryTexture(gcountdownTextures[i], NULL, NULL, &w, &h);
		gcountdownRects[i].w = w;
		gcountdownRects[i].h = h;
		gcountdownRects[i].x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
		gcountdownRects[i].y = gPlayarea.y + gPlayarea.h / 2 - h / 2;
	}

	//textures for pause menu
	gtResume = makeTextTexture(r, gMenuFont, "Continue", TEXT_COLOR, BG_COLOR, BLENDED);
	gtResumeSel = makeTextTexture(r, gMenuFont, "Continue", Select_Color, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtResume, NULL, NULL, &w, &h);
	gResume_rect.w = w;
	gResume_rect.h = h;
	gResume_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gResume_rect.y = SCREEN_HEIGHT / 2 - h - 1; 
	gtQuit = makeTextTexture(r, gMenuFont, "Quit", TEXT_COLOR, BG_COLOR, BLENDED);
	gtQuitSel = makeTextTexture(r, gMenuFont, "Quit", Select_Color, BG_COLOR, BLENDED);
	SDL_QueryTexture(gtQuit, NULL, NULL, &w, &h);
	gQuit_rect.w = w;
	gQuit_rect.h = h;
	gQuit_rect.x = SCREEN_WIDTH / 2 - w / 2;
	gQuit_rect.y = SCREEN_HEIGHT / 2 + 1;
	gPausedOption = 0;
}

void freeGameResources(void)
{
	int i;

	if (gMusic != NULL)
		Mix_FreeMusic(gMusic);

	if (gCountdownSound != NULL)
		Mix_FreeChunk(gCountdownSound);

	if (gHitSound != NULL)
		Mix_FreeChunk(gHitSound);

	if (gDeathSound != NULL)
		Mix_FreeChunk(gDeathSound);

	if (gWinSound != NULL)
		Mix_FreeChunk(gWinSound);

	if (gtLevelNumber != NULL)
		SDL_DestroyTexture(gtLevelNumber);

	if (gtHitPoints != NULL)
		SDL_DestroyTexture(gtHitPoints);

	if (gtBoost != NULL)
		SDL_DestroyTexture(gtBoost);

	if (gtTime != NULL)
		SDL_DestroyTexture(gtTime);

	if (gtmoreAI != NULL)
	SDL_DestroyTexture(gtmoreAI);

	if (gtrecover != NULL)
	SDL_DestroyTexture(gtrecover);

	if (gthardGoal != NULL)
	SDL_DestroyTexture(gthardGoal);

	if (gtfasterAI != NULL)
	SDL_DestroyTexture(gtfasterAI);

	if (gtfasterrecharge != NULL)
	SDL_DestroyTexture(gtfasterrecharge);

	if (gtyellowbox != NULL)
	SDL_DestroyTexture(gtyellowbox);

	if (gtgameover != NULL)
		SDL_DestroyTexture(gtgameover);

	if (gtvictory != NULL)
		SDL_DestroyTexture(gtvictory);

	if (gtcontinue != NULL)
		SDL_DestroyTexture(gtcontinue);

	if (gtResume != NULL)
		SDL_DestroyTexture(gtResume);

	if (gtResumeSel != NULL)
		SDL_DestroyTexture(gtResumeSel);

	if (gtQuit != NULL)
		SDL_DestroyTexture(gtQuit);

	if (gtQuitSel != NULL)
		SDL_DestroyTexture(gtQuitSel);

	for (i = 0; i < 3; i++)
	{
		if (gcountdownTextures[i] != NULL)
			SDL_DestroyTexture(gcountdownTextures[i]);
	}
}


void resetGame(void)
{
	gLevel = 1;
	gHitPoints = 3;
	gBoostPool = MAX_BOOST;
	gBoostDown = BOOST_DOWN_START;
	gBoostRecharge = BOOST_RECHARGE_START;
	gNumAIs = 1;
	gGoalBox.rand_direction = 1;
	gCurrentLevelTime = 0;
	gPreviousLevelTime = 0;
	gPaused = false;
	Mix_PlayMusic(gMusic, -1);
}

void newGame(SDL_Renderer *r)
{

	char buffer[100];
	int w, h, i, j;
	unsigned long int m, s;
	short int done;
	double playerbuffer = PLAYER_BUFFER*PLAYER_BUFFER;
	double distance;
	int edgebuffer = 10; //pixel buffer from the edge


	gBoostPool = MAX_BOOST;
	gnumUpgrades = 0;

	if (gtLevelNumber != NULL)
		SDL_DestroyTexture(gtLevelNumber);
	sprintf_s(buffer, sizeof(buffer), "Level: %d", gLevel);
	gtLevelNumber = makeTextTexture(r, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
	SDL_QueryTexture(gtLevelNumber, NULL, NULL, &w, &h);
	gLevel_rect.x = PLAYAREA_PADDING;
	gLevel_rect.y = PLAYAREA_PADDING;
	gLevel_rect.w = w;
	gLevel_rect.h = h;

	if (gtHitPoints != NULL)
		SDL_DestroyTexture(gtHitPoints);
	sprintf_s(buffer, sizeof(buffer), "Hit Points: %d", gHitPoints);
	gtHitPoints = makeTextTexture(r, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
	SDL_QueryTexture(gtHitPoints, NULL, NULL, &w, &h);
	gHitPoints_rect.x = PLAYAREA_PADDING + gLevel_rect.w + 20;
	gHitPoints_rect.y = PLAYAREA_PADDING;
	gHitPoints_rect.w = w;
	gHitPoints_rect.h = h;

	gBoost_rect.x = gHitPoints_rect.x + gHitPoints_rect.w + 20;

	gBoostmeter_rect.x = gBoost_rect.x + gBoost_rect.w + 5;
	gBoostmeter_rect.y = PLAYAREA_PADDING;
	gBoostmeter_rect.w = 206;
	gBoostmeter_rect.h = gBoost_rect.h - 5;

	gBoostdeplet_rect.y = gBoostmeter_rect.y + 3;
	gBoostdeplet_rect.h = gBoostmeter_rect.h - 6;
	gBoostdeplet_rect.x = gBoostmeter_rect.x; //just to initalize 
	gBoostdeplet_rect.w = gBoostmeter_rect.w; //just to initalize 

	gPreviousLevelTime = gPreviousLevelTime + gCurrentLevelTime;
	gCurrentLevelTime = 0;
	m = gPreviousLevelTime / 60;
	s = gPreviousLevelTime - m * 60;

	if (gtTime != NULL)
		SDL_DestroyTexture(gtTime);
	sprintf_s(buffer, sizeof(buffer), "Time: %02d:%02d  /  %02d:%02d", 0, 0, m, s);
	gtTime = makeTextTexture(r, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
	SDL_QueryTexture(gtTime, NULL, NULL, &w, &h);
	gTime_rect.x = gBoostmeter_rect.x + gBoostmeter_rect.w + 20;
	gTime_rect.y = PLAYAREA_PADDING;
	gTime_rect.w = w;
	gTime_rect.h = h;

	gPlayerBox.x = rnd(gPlayarea.w - 2 * edgebuffer) + gPlayarea.x + edgebuffer;
	gPlayerBox.y = rnd(gPlayarea.h - 2 * edgebuffer) + gPlayarea.y + edgebuffer;
	gPlayerBox.color = WHITE;
	gPlayerBox.invernable = false;
	gPlayerBox.inv_draw = false;
	gPlayerBox.inv_count = 0;
	gPlayerBox.inv_flash_count = 0;

	done = 0; j = 0; 
	while (done == 0 && j < MAX_BUFFER_SEARCH)
	{
		gGoalBox.x = rnd(gPlayarea.w - 2 * edgebuffer) + gPlayarea.x + edgebuffer;
		gGoalBox.y = rnd(gPlayarea.h - 2 * edgebuffer) + gPlayarea.y + edgebuffer;
		distance = SDL_pow((double)(gPlayerBox.x - gGoalBox.x), 2) + SDL_pow((double)(gPlayerBox.y - gGoalBox.y), 2);  //keeping everything squared to save time 
		if (distance > playerbuffer)
			done = 1;
		j++;
	}
	gGoalBox.direction = rnd(numMoveDirection);
#ifdef _DEBUG_BUILD_
	printf("Level %d (Boost Down: %f Boost Recharge: %f):\n", gLevel, gBoostDown, gBoostRecharge);
	printf("Found Goal Box location in %d loops.\n", j);
#endif
	for (i = 0; i < gNumAIs; i++)
	{
		done = 0; j = 0;
		while (done == 0 && j < MAX_BUFFER_SEARCH)
		{
			gEnemyBox[i].x = rnd(gPlayarea.w - 2 * edgebuffer) + gPlayarea.x + edgebuffer;
			gEnemyBox[i].y = rnd(gPlayarea.h - 2 * edgebuffer) + gPlayarea.y + edgebuffer;
			distance = SDL_pow((double)(gPlayerBox.x - gEnemyBox[i].x), 2) + SDL_pow((double)(gPlayerBox.y - gEnemyBox[i].y), 2);
			if (distance > playerbuffer)
				done = 1;
			j++;
		}
		gEnemyBox[i].direction = rnd(numMoveDirection);
#ifdef _DEBUG_BUILD_
		printf("Found Enemy Box %d location in %d loops.\n", i, j);
#endif
	}

	gGameState = COUNTDOWN;
	gCountdownSecCount = 0;
	gStartLevelTick = SDL_GetTicks();
	Mix_PlayChannel(-1, gCountdownSound, 0);
}



bool gameKeyboard(SDL_Renderer *r)
{
	int movespeed = 2; //pixels moved per frame 
	bool isedgehit = false;

	if (gPaused)
		return(isedgehit); //if paused just skip it

	const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

	switch (gGameState)
	{
	case GAMEPLAY:
		if ( (currentKeyStates[SDL_SCANCODE_LCTRL] || currentKeyStates[SDL_SCANCODE_RCTRL]) && //boost pressed and
			(currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_DOWN] ||
			currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_RIGHT]) ) //any of the movement keys 
		{
			if (gBoostPool > 0)
				movespeed = 4;

			gBoostPool = gBoostPool - gBoostDown;
			if (gBoostPool < 0)
				gBoostPool = 0;
		}
		else
		{
			gBoostPool = gBoostPool + gBoostRecharge;
			if (gBoostPool > MAX_BOOST)
				gBoostPool = MAX_BOOST;
		}

		if (currentKeyStates[SDL_SCANCODE_UP] && !currentKeyStates[SDL_SCANCODE_DOWN])
			gPlayerBox.y = gPlayerBox.y - movespeed;
		else if (currentKeyStates[SDL_SCANCODE_DOWN] && !currentKeyStates[SDL_SCANCODE_UP])
			gPlayerBox.y = gPlayerBox.y + movespeed;
		if (currentKeyStates[SDL_SCANCODE_LEFT] && !currentKeyStates[SDL_SCANCODE_RIGHT])
			gPlayerBox.x = gPlayerBox.x - movespeed;
		else if (currentKeyStates[SDL_SCANCODE_RIGHT] && !currentKeyStates[SDL_SCANCODE_LEFT])
			gPlayerBox.x = gPlayerBox.x + movespeed;

		if (gPlayerBox.x < gLeftEdge)
		{
			gPlayerBox.x = gLeftEdge;
			isedgehit = true;
		}
		else if (gPlayerBox.x > gRightEdge)
		{
			gPlayerBox.x = gRightEdge;
			isedgehit = true;
		}

		if (gPlayerBox.y < gTopEdge)
		{
			gPlayerBox.y = gTopEdge;
			isedgehit = true;
		}
		else if (gPlayerBox.y > gBottomEdge)
		{
			gPlayerBox.y = gBottomEdge;
			isedgehit = true;
		}
		break;
	//case GAMEOVER:  //moved to new function 
	//	if (currentKeyStates[SDL_SCANCODE_RETURN])
	//	{
	//		gPlayerBox.invernable = false;
	//		isedgehit = false;
	//		//resetGame(); //game is over so reset back to starting values
	//		//newGame(r); //then start a new game 
	//		MoveToShell();
	//	}
	//	break;
	case VICTORY:
		if (currentKeyStates[SDL_SCANCODE_RETURN])
		{
			gPlayerBox.invernable = false;
			isedgehit = false;
			newGame(r);
		}
		break;
	default:
		break;
	}

	return(isedgehit);
}

void checkEndGame(SDL_Renderer *r)
{
	if (gGameState == GAMEOVER)
	{
		gPlayerBox.invernable = false;
		MoveToShell(gLevel-1,gPreviousLevelTime,r);
	}

}

bool pauseGame(SDL_Keycode keycode)
{
	bool forceEdgeHit = false;

	switch (keycode)
	{
	case SDLK_ESCAPE:
		if (gGameState == GAMEPLAY)
		{
			gPaused = !gPaused;
			gPausedOption = 0;
		}
		break;
	case SDLK_UP:
		if (gPaused && gPausedOption > 0)
			gPausedOption--;
		break;
	case SDLK_DOWN:
		if (gPaused && gPausedOption < 1)
			gPausedOption++;
		break;
	case SDLK_RETURN:
		if (gPaused && gPausedOption == 0)
			gPaused = false;
		else if (gPaused && gPausedOption == 1)
		{
			gHitPoints = 0; //kill the player
			forceEdgeHit = true;
			gPaused = false;
			gPausedOption = 0;
		}
		break;
	default:
		break;
	}

	return(forceEdgeHit);
}

void gameLogic(SDL_Renderer *r, bool isedgehit)
{
	int i, w, h;
	char buffer[20];
	SDL_Rect temprect;
	SDL_Rect playerrect = {
		gPlayerBox.x,
		gPlayerBox.y,
		gPlayerBox.w,
		gPlayerBox.h
	};

	if (gPaused)
		return; //if it is paused just skip it 

	//process game movement and win / lose conditions 
	//check for collision 
	if (!gPlayerBox.invernable && !isedgehit)
	{
		for (i = 0; i < gNumAIs; i++)
		{
			if (SDL_HasIntersection(&playerrect, copyToSDLRect(&gEnemyBox[i], &temprect)) == SDL_TRUE)
			{
				isedgehit = true; // treat like a collision
				break; //don't need to check anymore 
			}
		}
	}
		


	if (!gPlayerBox.invernable && gGameState == GAMEPLAY &&
		SDL_HasIntersection(&playerrect, copyToSDLRect(&gGoalBox, &temprect)) == SDL_TRUE)
	{
		gGameState = VICTORY;
		//Mix_HaltMusic();
		Mix_PlayChannel(-1, gWinSound, 0);
		gcontinue_rect.y = gvictory_rect.y + gvictory_rect.h + END_TEXT_SPACING; //adjust the continue rect under victory

		if (gLevel < 100)
		{
			gLevel++;
			gnumUpgrades = 0;
			if ((gLevel % 10) == 0)
			{
				gBoostRecharge = gBoostRecharge + BOOST_RECHARGE_UPGRADE;
			}
			if ((gLevel % 5) == 0)
			{
				gHitPoints++;
				gBoostDown = gBoostDown - BOOST_DOWN_UPGRADE;
				gupgradeTextures[gnumUpgrades] = gtrecover;
				gnumUpgrades++;
				if ((gLevel % 10) == 0)
				{
					gBoostRecharge = gBoostRecharge + BOOST_RECHARGE_UPGRADE;
					gupgradeTextures[gnumUpgrades] = gtfasterrecharge;
					gnumUpgrades++;
				}
			}
			if ((gLevel % 2) == 0 && gNumAIs < MAX_AI_BOXES - 1)
			{
				gNumAIs++;
				gupgradeTextures[gnumUpgrades] = gtmoreAI;
				gnumUpgrades++;
			}
			if ((gLevel % 3) == 0)
			{
				gGoalBox.rand_direction = gGoalBox.rand_direction + 1;
				gupgradeTextures[gnumUpgrades] = gthardGoal;
				gnumUpgrades++;
			}
			if ((gLevel % 10) == 2 && gLevel > 10)
			{
				gupgradeTextures[gnumUpgrades] = gtfasterAI;
				gnumUpgrades++;
			}
			if (gLevel == 10)
			{
				gupgradeTextures[gnumUpgrades] = gtyellowbox;
				gnumUpgrades++;
			}
			for (i = 0; i < gnumUpgrades; i++)
			{
				SDL_QueryTexture(gupgradeTextures[i], NULL, NULL, &w, &h);
				gupgradeRects[i].w = w;
				gupgradeRects[i].h = h;
				gupgradeRects[i].x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
				if (i == 0)
					gupgradeRects[0].y = gcontinue_rect.y + gcontinue_rect.h + END_TEXT_SPACING;
				else
					gupgradeRects[i].y = gupgradeRects[i - 1].y + gupgradeRects[i - 1].h + END_TEXT_SPACING;
			}
		}
	}

	if (isedgehit && !gPlayerBox.invernable && gGameState == GAMEPLAY)
	{
		if (gHitPoints <= 0)
		{
			gGameState = GAMEOVER;
			Mix_HaltMusic();
			Mix_PlayChannel(-1, gDeathSound, 0);

			if (gtHitPoints != NULL)
				SDL_DestroyTexture(gtHitPoints);
			gtHitPoints = makeTextTexture(r, gSmallFont, "Hit Points:  ", TEXT_COLOR, BG_COLOR, SHADED);
			SDL_QueryTexture(gtHitPoints, NULL, NULL, &w, &h);
			gHitPoints_rect.w = w;
			gHitPoints_rect.h = h;
			gcontinue_rect.y = ggameover_rect.y + ggameover_rect.h + END_TEXT_SPACING; //adjust the continue rect to new location
		}
		else
		{
			Mix_PlayChannel(-1, gHitSound, 0);
			gPlayerBox.color = RED;
			gHitPoints--;
			gPlayerBox.invernable = true;
			gPlayerBox.inv_count = 0;
			gPlayerBox.inv_draw = true;
			gPlayerBox.inv_flash_count = 0;

			if (gtHitPoints != NULL)
				SDL_DestroyTexture(gtHitPoints);
			sprintf_s(buffer, sizeof(buffer), "Hit Points: %d", gHitPoints);
			gtHitPoints = makeTextTexture(r, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
			SDL_QueryTexture(gtHitPoints, NULL, NULL, &w, &h);
			gHitPoints_rect.w = w;
			gHitPoints_rect.h = h;
		}
	}

	if (gPlayerBox.invernable)
	{
		gPlayerBox.inv_count++;
		gPlayerBox.inv_flash_count++;
		if (gPlayerBox.inv_flash_count >= INV_FLASH_FRAME)
		{
			gPlayerBox.inv_flash_count = 0;
			gPlayerBox.color = WHITE;
			gPlayerBox.inv_draw = !gPlayerBox.inv_draw;
		}

		if (gPlayerBox.inv_count >= INV_FRAMES)
		{
			gPlayerBox.inv_count = 0;
			gPlayerBox.invernable = false;
		}
	}

	processBoxMovement(); //process box movement


	//check countdown
	if (gGameState == COUNTDOWN)
		processCountdown();
}

void processCountdown(void)
{
	Uint32 currentTick;

	currentTick = SDL_GetTicks() - gStartLevelTick;
	if (currentTick >= 1000 && currentTick < 2000)
		gCountdownSecCount = 1;
	else if (currentTick >= 2000 && currentTick < 3000)
		gCountdownSecCount = 2;
	else if (currentTick >= 3000)
	{
		gGameState = GAMEPLAY;
		//Mix_PlayMusic(gMusic, -1);
	}
}

void processBoxMovement()
{
	int i;
	if (gGameState == GAMEPLAY || gGameState == GAMEOVER)
	{
		moveAIBox(&gGoalBox);
		for (i = 0; i < gNumAIs; i++)
			moveAIBox(&gEnemyBox[i]);
	}
}

void drawPlayArea(SDL_Renderer *r)
{
	int i;
	SDL_Rect tempbox;
	SDL_Rect playerrect = {
		gPlayerBox.x,
		gPlayerBox.y,
		gPlayerBox.w,
		gPlayerBox.h
	};

	SDL_RenderCopy(r, gtLevelNumber, NULL, &gLevel_rect);
	SDL_RenderCopy(r, gtHitPoints, NULL, &gHitPoints_rect);
	SDL_RenderCopy(r, gtBoost, NULL, &gBoost_rect);
	DrawBox(r, &gBoostmeter_rect, GREEN);
	if (gBoostPool < MAX_BOOST) //draw the boost meeter depletion 
	{
		gBoostdeplet_rect.w = (int)((MAX_BOOST - gBoostPool) / MAX_BOOST * 200);
		gBoostdeplet_rect.x = gBoostmeter_rect.x + gBoostmeter_rect.w - 3 - gBoostdeplet_rect.w;
		DrawBox(r, &gBoostdeplet_rect, BLACK);
	}
	SDL_RenderCopy(r, gtTime, NULL, &gTime_rect); //draw the timer

	DrawBox(r, &gBorder, gPlayerBox.color);  //draw border
	DrawBox(r, &gPlayarea, BLACK); //draw playfield


	//draw the dynamic boxes
	DrawBox(r, copyToSDLRect(&gGoalBox, &tempbox), gGoalBox.color);
	if (gGameState != VICTORY && gGameState != NOGAME)
	{
		for (i = 0; i < gNumAIs; i++)
			DrawBox(r, copyToSDLRect(&gEnemyBox[i], &tempbox), gEnemyBox[i].color);
	}

	//if the logic is right draw the player box 


	if (gPlayerBox.invernable && gPlayerBox.inv_draw)
		DrawBox(r, &playerrect, gPlayerBox.color);
	else if (!gPlayerBox.invernable && gGameState != GAMEOVER && gGameState != NOGAME)
		DrawBox(r, &playerrect, gPlayerBox.color);

	switch (gGameState) // draw overlay text based on the game state
	{
	case GAMEOVER:
		SDL_RenderCopy(r, gtgameover, NULL, &ggameover_rect); //if the game is over draw the Game Over text 		
		SDL_RenderCopy(r, gtcontinue, NULL, &gcontinue_rect);
		break;
	case VICTORY:
		SDL_RenderCopy(r, gtvictory, NULL, &gvictory_rect); //if the game is won draw the Victory text 		
		SDL_RenderCopy(r, gtcontinue, NULL, &gcontinue_rect);
		for (i = 0; i < gnumUpgrades; i++)
			SDL_RenderCopy(r, gupgradeTextures[i], NULL, &gupgradeRects[i]);
		break;
	case COUNTDOWN:
		SDL_RenderCopy(r, gcountdownTextures[gCountdownSecCount], NULL, &gcountdownRects[gCountdownSecCount]);
		break;
	default:
		break;
	}

	if (gPaused)
	{
		DrawBox(r, NULL, TRANSPARENT_BLACK); //gray out the screen
		if (gPausedOption == 0) //draw the options on the screen
		{
			SDL_RenderCopy(r, gtResumeSel, NULL, &gResume_rect);
			SDL_RenderCopy(r, gtQuit, NULL, &gQuit_rect);
		}
		else
		{
			SDL_RenderCopy(r, gtResume, NULL, &gResume_rect);
			SDL_RenderCopy(r, gtQuitSel, NULL, &gQuit_rect);
		}
	}

}

void updateGameTimer(SDL_Renderer *r)
{
	char buffer[40];
	unsigned long int m1, s1, m2, s2, combine_t;
	int w, h; 

	if (gGameState == GAMEPLAY && !gPaused)
	{
		gCurrentLevelTime++;
		m1 = gCurrentLevelTime / 60;
		s1 = gCurrentLevelTime - m1 * 60;
		combine_t = gCurrentLevelTime + gPreviousLevelTime;
		m2 = combine_t / 60;
		s2 = combine_t - m2 * 60;
		if (gtTime != NULL)
			SDL_DestroyTexture(gtTime);
		sprintf_s(buffer, sizeof(buffer), "Time: %02d:%02d  /  %02d:%02d", m1, s1, m2, s2);
		gtTime = makeTextTexture(r, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
		SDL_QueryTexture(gtTime, NULL, NULL, &w, &h);
		gTime_rect.w = w;
		gTime_rect.h = h;
	}

}

void moveAIBox(struct AIBox *ai)
{
	switch (ai->aitype)
	{
	case RANDOM:
		//first check if direction should be switched due to collision 
		if (checkAIBoxDirection(ai) == 1)
		{
			//direction has changed so only move the box
			changeAIBoxCoordinates(ai);
		}
		else
		{
			//move the box and then see if the direction should be changed for the next move 
			changeAIBoxCoordinates(ai);
			if (rnd(100) < ai->rand_direction)
				ai->direction = rnd(numMoveDirection);
		}
		break;
	case SEEK:
		if (ai->x < gPlayerBox.x)
			ai->x = ai->x + ai->speed;
		else if (ai->x > gPlayerBox.x)
			ai->x = ai->x - ai->speed;

		if (ai->y < gPlayerBox.y)
			ai->y = ai->y + ai->speed;
		else if (ai->y > gPlayerBox.y)
			ai->y = ai->y - ai->speed;

		break;
	case NONE:
	default:
		break;
	}
}

int checkAIBoxDirection(struct AIBox *ai)
{
	int result = 0; //0 if box didn't change direction; 1 if it does 
	struct AIBox temp;
	temp.x = ai->x;
	temp.y = ai->y;
	temp.w = ai->w;
	temp.h = ai->h;
	temp.direction = ai->direction;
	temp.speed = ai->speed;
	temp.rand_direction = 0;
	temp.color = BLACK;

	changeAIBoxCoordinates(&temp); //move the temp box and then check and see if there is an issue 

	if (temp.x <= gLeftEdge) //change the west to east 
	{
		result = 1;
		switch (ai->direction)
		{
		case WEST:
			ai->direction = EAST;
			break;
		case NORTHWEST:
			ai->direction = NORTHEAST;
			break;
		case SOUTHWEST:
			ai->direction = SOUTHEAST;
			break;
		default:
			break;
		}
	}

	if (temp.x >= gRightEdge) //change the east to west 
	{
		result = 1;
		switch (ai->direction)
		{
		case EAST:
			ai->direction = WEST;
			break;
		case NORTHEAST:
			ai->direction = NORTHWEST;
			break;
		case SOUTHEAST:
			ai->direction = SOUTHWEST;
			break;
		default:
			break;
		}
	}

	if (temp.y <= gTopEdge) //change the north to south 
	{
		result = 1;
		switch (ai->direction)
		{
		case NORTH:
			ai->direction = SOUTH;
			break;
		case NORTHWEST:
			ai->direction = SOUTHWEST;
			break;
		case NORTHEAST:
			ai->direction = SOUTHEAST;
			break;
		default:
			break;
		}
	}

	if (temp.y >= gBottomEdge) //change the south to north
	{
		result = 1;
		switch (ai->direction)
		{
		case SOUTH:
			ai->direction = NORTH;
			break;
		case SOUTHWEST:
			ai->direction = NORTHWEST;
			break;
		case SOUTHEAST:
			ai->direction = NORTHEAST;
			break;
		default:
			break;
		}
	}
	return result;
}

void changeAIBoxCoordinates(struct AIBox *ai)
{
	int move = ai->speed;

	switch (ai->direction)
	{
	case NORTH:
		ai->y = ai->y - move;
		break;
	case SOUTH:
		ai->y = ai->y + move;
		break;
	case EAST:
		ai->x = ai->x + move;
		break;
	case WEST:
		ai->x = ai->x - move;
		break;
	case NORTHEAST:
		ai->x = ai->x + move;
		ai->y = ai->y - move;
		break;
	case NORTHWEST:
		ai->x = ai->x - move;
		ai->y = ai->y - move;
		break;
	case SOUTHEAST:
		ai->x = ai->x + move;
		ai->y = ai->y + move;
		break;
	case SOUTHWEST:
		ai->x = ai->x - move;
		ai->y = ai->y + move;
		break;
	default:
		break;
	}
}