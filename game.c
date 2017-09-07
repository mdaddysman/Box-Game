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
short int gIsCountdown, gCountdownSecCount; //is the countdown running?
Uint32 gStartLevelTick;
Mix_Music *gMusic;
Mix_Chunk *gCountdownSound;
SDL_Texture *gtLevelNumber, *gtHitPoints, *gtBoost, *gtTime;
SDL_Rect gLevel_rect, gHitPoints_rect, gBoost_rect, gBoostmeter_rect, gBoostdeplet_rect, gTime_rect, gPlayerBox, gBorder, gPlayarea;
struct AIBox gGoalBox, gEnemyBox[MAX_AI_BOXES];

//extern fonts
extern TTF_Font *gSmallFont, *gLargeFont;

void loadGameResources(SDL_Renderer *r)
{
	int i, w, h;
	//initalize global variables 
	gMusic = NULL;
	gCountdownSound = NULL;
	gtLevelNumber = NULL;
	gtHitPoints = NULL;
	gtBoost = NULL;
	gtTime = NULL;


	gPlayerBox.x = 200;
	gPlayerBox.y = 200;
	gPlayerBox.w = PLAYERBOX_WIDTH;
	gPlayerBox.h = PLAYERBOX_HEIGHT;

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
	//end initalize global variables 

	//load global resources
	gMusic = Mix_LoadMUS(GAME_MUSIC);
	gCountdownSound = Mix_LoadWAV(COUNTDOWN_SOUND);

	gtBoost = makeTextTexture(r, gSmallFont, "Boost:", TEXT_COLOR, BG_COLOR, SHADED);
	SDL_QueryTexture(gtBoost, NULL, NULL, &w, &h);
	gBoost_rect.x = gHitPoints_rect.x + gHitPoints_rect.w + 20;
	gBoost_rect.y = PLAYAREA_PADDING;
	gBoost_rect.w = w;
	gBoost_rect.h = h;
}

void freeGameResources(void)
{
	if (gMusic != NULL)
		Mix_FreeMusic(gMusic);

	if (gCountdownSound != NULL)
		Mix_FreeChunk(gCountdownSound);

	if (gtLevelNumber != NULL)
		SDL_DestroyTexture(gtLevelNumber);

	if (gtHitPoints != NULL)
		SDL_DestroyTexture(gtHitPoints);

	if (gtBoost != NULL)
		SDL_DestroyTexture(gtBoost);

	if (gtTime != NULL)
		SDL_DestroyTexture(gtTime);
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
}

void newGame(SDL_Renderer *r)
{

	char buffer[100];
	int w, h, i;
	unsigned long int m, s;
	short int done;
	double playerbuffer = PLAYER_BUFFER*PLAYER_BUFFER;
	double distance;
	int edgebuffer = 10; //pixel buffer from the edge


	gBoostPool = MAX_BOOST;

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

	done = 0;

	while (done == 0)
	{
		gGoalBox.x = rnd(gPlayarea.w - 2 * edgebuffer) + gPlayarea.x + edgebuffer;
		gGoalBox.y = rnd(gPlayarea.h - 2 * edgebuffer) + gPlayarea.y + edgebuffer;
		distance = SDL_pow((double)(gPlayerBox.x - gGoalBox.x), 2) + SDL_pow((double)(gPlayerBox.y - gGoalBox.y), 2);  //keeping everything squared to save time 
		if (distance > playerbuffer)
			done = 1;
	}
	gGoalBox.direction = rnd(numMoveDirection);

	for (i = 0; i < gNumAIs; i++)
	{
		done = 0;
		while (done == 0)
		{
			gEnemyBox[i].x = rnd(gPlayarea.w - 2 * edgebuffer) + gPlayarea.x + edgebuffer;
			gEnemyBox[i].y = rnd(gPlayarea.h - 2 * edgebuffer) + gPlayarea.y + edgebuffer;
			distance = SDL_pow((double)(gPlayerBox.x - gEnemyBox[i].x), 2) + SDL_pow((double)(gPlayerBox.y - gEnemyBox[i].y), 2);
			if (distance > playerbuffer)
				done = 1;
		}
		gEnemyBox[i].direction = rnd(numMoveDirection);
	}

	gIsCountdown = 1;
	gCountdownSecCount = 0;
	gStartLevelTick = SDL_GetTicks();
	Mix_PlayChannel(-1, gCountdownSound, 0);
}

short int checkEnemyCollision(void)
{
	int i;
	short int isHit = 0;
	SDL_Rect tempbox;

	for (i = 0; i < gNumAIs; i++)
	{
		if (SDL_HasIntersection(&gPlayerBox, copyToSDLRect(&gEnemyBox[i], &tempbox)) == SDL_TRUE)
		{
			isHit = 1; // treat like a collision
			break; //don't need to check anymore 
		}
	}

	return(isHit);
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
		gIsCountdown = 0;
		Mix_PlayMusic(gMusic, -1);
	}
}

void processBoxMovement(short int isVictory)
{
	int i;
	if (isVictory == 0 && gIsCountdown == 0)
	{
		moveAIBox(&gGoalBox);
		for (i = 0; i < gNumAIs; i++)
			moveAIBox(&gEnemyBox[i]);
	}
}

short int gameKeyboard(const Uint8 *currentKeyStates)
{
	int movespeed = 2; //pixels moved per frame 
	short int isedgehit = 0;

	if (currentKeyStates[SDL_SCANCODE_LCTRL] || currentKeyStates[SDL_SCANCODE_RCTRL])
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
		isedgehit = 1;
	}
	else if (gPlayerBox.x > gRightEdge)
	{
		gPlayerBox.x = gRightEdge;
		isedgehit = 1;
	}

	if (gPlayerBox.y < gTopEdge)
	{
		gPlayerBox.y = gTopEdge;
		isedgehit = 1;
	}
	else if (gPlayerBox.y > gBottomEdge)
	{
		gPlayerBox.y = gBottomEdge;
		isedgehit = 1;
	}

	return(isedgehit);
}

void drawPlayArea(SDL_Renderer *r, enum BoxColors playercolor, short int isVictory)
{
	SDL_Rect tempbox;
	int i;

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

	DrawBox(r, &gBorder, playercolor);  //draw border
	DrawBox(r, &gPlayarea, BLACK); //draw playfield


	//draw the dynamic boxes
	DrawBox(r, copyToSDLRect(&gGoalBox, &tempbox), gGoalBox.color);
	if (isVictory == 0)
	{
		for (i = 0; i < gNumAIs; i++)
			DrawBox(r, copyToSDLRect(&gEnemyBox[i], &tempbox), gEnemyBox[i].color);
	}
}

void updateGameTimer(SDL_Renderer *r)
{
	char buffer[40];
	unsigned long int m1, s1, m2, s2, combine_t;
	int w, h; 

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