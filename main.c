/*
 * Box Game - main.c
 * Box Game to demo SDL2 and simple AI
 * Forked from SDL Learning repo on 9/3/2017 
 * By Matthew K. Daddysman
 */

#include "main.h"

//global variables 
int gRandCount; // number of times the random number generator has been called 
int gLevel, gHitPoints, gBoostPool; //the current Level, Hit Points, and BoostPool of the player 
int gLeftEdge, gRightEdge, gTopEdge, gBottomEdge; //define the edges of the playfield for bounds checking 
Mix_Music *gMusic;
TTF_Font *gSmallFont, *gLargeFont;
SDL_Texture *gtLevelNumber, *gtHitPoints, *gtBoost;
SDL_Rect gLevel_rect, gHitPoints_rect, gBoost_rect, gBoostmeter_rect, gBoostdeplet_rect, gPlayerBox;
struct AIBox gGoalBox, gEnemyBox;


void newGame(SDL_Renderer *r, SDL_Rect *playarea)
{
	char buffer[100];
	int w, h;
	int edgebuffer = 10; //pixel buffer from the edge
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Color bg_color = { 0, 0, 0 };

	gBoostPool = MAX_BOOST;

	sprintf_s(buffer, sizeof(buffer), "Level: %d", gLevel);
	gtLevelNumber = makeTextTexture(r, gSmallFont, buffer, text_color, bg_color, SHADED);
	SDL_QueryTexture(gtLevelNumber, NULL, NULL, &w, &h);
	gLevel_rect.x = PLAYAREA_PADDING;
	gLevel_rect.y = PLAYAREA_PADDING;
	gLevel_rect.w = w;
	gLevel_rect.h = h;

	sprintf_s(buffer, sizeof(buffer), "Hit Points: %d", gHitPoints);
	gtHitPoints = makeTextTexture(r, gSmallFont, buffer, text_color, bg_color, SHADED);
	SDL_QueryTexture(gtHitPoints, NULL, NULL, &w, &h);
	gHitPoints_rect.x = PLAYAREA_PADDING + gLevel_rect.w + 20;
	gHitPoints_rect.y = PLAYAREA_PADDING;
	gHitPoints_rect.w = w;
	gHitPoints_rect.h = h;

	gtBoost = makeTextTexture(r, gSmallFont, "Boost:", text_color, bg_color, SHADED);
	SDL_QueryTexture(gtBoost, NULL, NULL, &w, &h);
	gBoost_rect.x = gHitPoints_rect.x + gHitPoints_rect.w + 20;
	gBoost_rect.y = PLAYAREA_PADDING;
	gBoost_rect.w = w;
	gBoost_rect.h = h;

	gBoostmeter_rect.x = gBoost_rect.x + gBoost_rect.w + 5;
	gBoostmeter_rect.y = PLAYAREA_PADDING;
	gBoostmeter_rect.w = 206;
	gBoostmeter_rect.h = gBoost_rect.h - 5;

	gBoostdeplet_rect.y = gBoostmeter_rect.y + 3;
	gBoostdeplet_rect.h = gBoostmeter_rect.h - 6;
	gBoostdeplet_rect.x = gBoostmeter_rect.x; //just to initalize 
	gBoostdeplet_rect.w = gBoostmeter_rect.w; //just to initalize 

	gPlayerBox.x = rnd(playarea->w - 2 * edgebuffer) + playarea->x + edgebuffer;
	gPlayerBox.y = rnd(playarea->h - 2 * edgebuffer) + playarea->y + edgebuffer;


	gGoalBox.x = rnd(playarea->w - 2 * edgebuffer) + playarea->x + edgebuffer;
	gGoalBox.y = rnd(playarea->h - 2 * edgebuffer) + playarea->y + edgebuffer;
	gGoalBox.direction = rnd(numMoveDirection);

	gEnemyBox.x = rnd(playarea->w - 2 * edgebuffer) + playarea->x + edgebuffer;
	gEnemyBox.y = rnd(playarea->h - 2 * edgebuffer) + playarea->y + edgebuffer;
	gEnemyBox.direction = rnd(numMoveDirection);

	Mix_PlayMusic(gMusic, -1);
}

int main(int argc, char* args[])
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	
	Mix_Chunk *wall = NULL;
	Mix_Chunk *death = NULL; 
	Mix_Chunk *win = NULL;
	
	SDL_Texture *tgameover = NULL;
	SDL_Texture *tvictory = NULL;
	SDL_Texture *tcontinue = NULL;

	char buffer[100];
	int quit = 0;
	int result = 0;
	short int isedgehit = 0;
	short int invernable = 0;
	short int inv_flash_count = 0;
	short int inv_draw = 0; 
	short int gameover = 0;
	short int victory = 0;
	int inv_count = 0; 

	SDL_Event e;
	enum BoxColors playercolor = WHITE;

	SDL_Rect tempbox = {0,0,0,0};
	SDL_Rect border = {
		PLAYAREA_PADDING,
		SCOREBAR_HEIGHT + PLAYAREA_PADDING,
		SCREEN_WIDTH - 2 * PLAYAREA_PADDING,
		SCREEN_HEIGHT - SCOREBAR_HEIGHT - 2 * PLAYAREA_PADDING
	};
	SDL_Rect playarea = {
		PLAYAREA_PADDING + PLAYAREA_BORDER,
		SCOREBAR_HEIGHT + PLAYAREA_PADDING + PLAYAREA_BORDER,
		SCREEN_WIDTH - 2 * PLAYAREA_PADDING - 2 * PLAYAREA_BORDER,
		SCREEN_HEIGHT - SCOREBAR_HEIGHT - 2 * PLAYAREA_PADDING - 2 * PLAYAREA_BORDER
	};
	
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Color bg_color = { 0, 0, 0 };
	SDL_Rect gameover_rect, victory_rect, continue_rect;
	int w, h;

	//initalize global variables
	gMusic = NULL;
	gSmallFont = NULL;
	gLargeFont = NULL;
	gtLevelNumber = NULL;
	gtHitPoints = NULL;
	gtBoost = NULL;
	gLevel = 1;
	gHitPoints = 3;
	gBoostPool = MAX_BOOST;

	gPlayerBox.x = 200;
	gPlayerBox.y = 200;
	gPlayerBox.w = 10;
	gPlayerBox.h = 10;

	gLeftEdge = playarea.x;
	gTopEdge = playarea.y;
	gRightEdge = playarea.x + playarea.w - gPlayerBox.w;
	gBottomEdge = playarea.y + playarea.h - gPlayerBox.h;
	
	gGoalBox.x = 200;
	gGoalBox.y = 200;
	gGoalBox.w = 10;
	gGoalBox.h = 10;
	gGoalBox.direction = NORTH;
	gGoalBox.rand_direction = 1;

	gEnemyBox.x = 200;
	gEnemyBox.y = 200;
	gEnemyBox.w = 10;
	gEnemyBox.h = 10;
	gEnemyBox.direction = NORTH;
	gEnemyBox.rand_direction = 1;
	//end initalize global variables

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! Error: %s\n", SDL_GetError());
		SDL_Quit();
		return(0);
	}

	if (MIX_INIT_MP3 != (result = Mix_Init(MIX_INIT_MP3)))
	{
		printf("SDL Mixer could not initialize! Code: %d Error: %s\n",result, SDL_GetError());
		SDL_Quit();
		return(0);
	}

	if (TTF_Init() < 0)
	{
		printf("SDL TTF could not initialize! Error: %s\n", SDL_GetError());
		Mix_Quit();
		SDL_Quit();
		return(0);
	}

	window = SDL_CreateWindow("Box Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		printf("Window could not be created! Error: %s\n", SDL_GetError());
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(0);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(0);
	}

	gSmallFont = TTF_OpenFont(FONT_FILE, 36);
	if (gSmallFont == NULL)
	{
		printf("Font could not be loaded Error:%s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(0);
	}

	gLargeFont = TTF_OpenFont(FONT_FILE, 144);
	if (gLargeFont == NULL)
	{
		printf("Large font could not be loaded Error:%s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(0);
	}

	seedrnd();
	
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	gMusic = Mix_LoadMUS(GAME_MUSIC);
	wall = Mix_LoadWAV(WALL_SOUND);
	death = Mix_LoadWAV(DEATH_SOUND);
	win = Mix_LoadWAV(WIN_SOUND);

	tgameover = makeTextTexture(renderer, gLargeFont, "GAME OVER", text_color, bg_color, BLENDED);
	SDL_QueryTexture(tgameover, NULL, NULL, &w, &h);
	gameover_rect.w = w;
	gameover_rect.h = h;
	gameover_rect.x = playarea.x + playarea.w / 2 - w / 2;
	gameover_rect.y = playarea.y + playarea.h / 2 - h / 2;

	tvictory = makeTextTexture(renderer, gLargeFont, "VICTORY", text_color, bg_color, BLENDED);
	SDL_QueryTexture(tvictory, NULL, NULL, &w, &h);
	victory_rect.w = w;
	victory_rect.h = h;
	victory_rect.x = playarea.x + playarea.w / 2 - w / 2;
	victory_rect.y = playarea.y + playarea.h / 2 - h / 2;

	tcontinue = makeTextTexture(renderer, gSmallFont, "Press ENTER to continue", text_color, bg_color, BLENDED);
	SDL_QueryTexture(tcontinue, NULL, NULL, &w, &h);
	continue_rect.w = w;
	continue_rect.h = h;
	continue_rect.x = playarea.x + playarea.w / 2 - w / 2;
	continue_rect.y = playarea.y + playarea.h / 2 - h / 2;
		
	newGame(renderer,&playarea); //start to configure a new game 

	while (quit == 0)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = 1;
		}
		

		const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

		if (gameover == 0 && victory == 0)
		{
			isedgehit = gameKeyboard(currentKeyStates);
		}
		else if (gameover == 1)
		{
			if (currentKeyStates[SDL_SCANCODE_RETURN])
			{
				gLevel = 1;
				gHitPoints = 3;
				gameover = 0;
				invernable = 0;
				isedgehit = 0;
				newGame(renderer, &playarea);
				continue;
			}

		}
		else if (victory == 1)
		{
			if (currentKeyStates[SDL_SCANCODE_RETURN])
			{
				gLevel++;
				victory = 0;
				invernable = 0;
				isedgehit = 0;
				newGame(renderer, &playarea);
				continue;
			}
				
		}

		//check for collision 
		if (invernable == 0 && SDL_HasIntersection(&gPlayerBox, copyToSDLRect(&gEnemyBox, &tempbox)) == SDL_TRUE)
		{
			isedgehit = 1; // treat like a collision
		}

		if (invernable == 0 && victory == 0 && SDL_HasIntersection(&gPlayerBox, copyToSDLRect(&gGoalBox, &tempbox)) == SDL_TRUE)
		{
			victory = 1;
			Mix_HaltMusic();
			Mix_PlayChannel(-1, win, 0);
		}

		if (isedgehit == 1 && invernable == 0 && gameover == 0 && victory == 0)
		{
			if (gHitPoints <= 0)
			{
				gameover = 1;
				Mix_HaltMusic();
				Mix_PlayChannel(-1, death, 0);
				gtHitPoints = makeTextTexture(renderer, gSmallFont, "Hit Points:  ", text_color, bg_color, SHADED);
				SDL_QueryTexture(gtHitPoints, NULL, NULL, &w, &h);
				gHitPoints_rect.w = w;
				gHitPoints_rect.h = h;
			}
			else
			{
				Mix_PlayChannel(-1, wall, 0);
				playercolor = RED;
				gHitPoints--;
				invernable = 1;
				inv_count = 0;
				inv_draw = 1;
				inv_flash_count = 0;
				sprintf_s(buffer, sizeof(buffer), "Hit Points: %d", gHitPoints);
				gtHitPoints = makeTextTexture(renderer, gSmallFont, buffer, text_color, bg_color,SHADED);
				SDL_QueryTexture(gtHitPoints, NULL, NULL, &w, &h);
				gHitPoints_rect.w = w;
				gHitPoints_rect.h = h;
			}
		}

		if (invernable == 1)
		{
			inv_count++;
			inv_flash_count++;
			if (inv_flash_count >= INV_FLASH_FRAME)
			{
				inv_flash_count = 0;
				playercolor = WHITE;
				if (inv_draw == 1)
					inv_draw = 0;
				else
					inv_draw = 1;
			}

			if (inv_count >= INV_FRAMES)
			{
				inv_count = 0;
				invernable = 0;
			}
		}

		//process box movement
		if (victory == 0)
		{
			moveAIBox(&gGoalBox);
			moveAIBox(&gEnemyBox);
		}

		//clear the screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		//draw the header
		SDL_RenderCopy(renderer, gtLevelNumber, NULL, &gLevel_rect);
		SDL_RenderCopy(renderer, gtHitPoints, NULL, &gHitPoints_rect);
		SDL_RenderCopy(renderer, gtBoost, NULL, &gBoost_rect);
		DrawBox(renderer, &gBoostmeter_rect, GREEN);
		if (gBoostPool < MAX_BOOST)
		{
			gBoostdeplet_rect.w = MAX_BOOST-gBoostPool;  
			gBoostdeplet_rect.x = gBoostmeter_rect.x+gBoostmeter_rect.w-3-gBoostdeplet_rect.w; //just to initalize 
			DrawBox(renderer, &gBoostdeplet_rect, BLACK);			
		}
		//draw playfield
		DrawBox(renderer, &border, playercolor);
		DrawBox(renderer, &playarea, BLACK);
		//draw the dynamic boxes
		DrawBox(renderer, copyToSDLRect(&gGoalBox, &tempbox), BLUE);
		if (victory == 0)
			DrawBox(renderer, copyToSDLRect(&gEnemyBox, &tempbox), ORANGE);
		if (invernable == 1 && inv_draw == 1)
			DrawBox(renderer, &gPlayerBox, playercolor);
		else if (invernable == 0 && gameover == 0)
			DrawBox(renderer, &gPlayerBox, playercolor);

		if (gameover == 1)
		{
			SDL_RenderCopy(renderer, tgameover, NULL, &gameover_rect); //if the game is over draw the Game Over text 
			continue_rect.y = gameover_rect.y + gameover_rect.h + 10;
			SDL_RenderCopy(renderer, tcontinue, NULL, &continue_rect);
		}
		if (victory == 1)
		{
			SDL_RenderCopy(renderer, tvictory, NULL, &victory_rect); //if the game is won draw the Victory text 
			continue_rect.y = victory_rect.y + victory_rect.h + 10;
			SDL_RenderCopy(renderer, tcontinue, NULL, &continue_rect);
		}
		SDL_RenderPresent(renderer); //present the frame 
	}

	Mix_HaltMusic();
	Mix_FreeMusic(gMusic);
	Mix_FreeChunk(wall);
	Mix_FreeChunk(death);
	Mix_FreeChunk(win);

	SDL_DestroyTexture(gtLevelNumber);
	SDL_DestroyTexture(gtHitPoints);
	SDL_DestroyTexture(gtBoost);
	SDL_DestroyTexture(tgameover);	
	SDL_DestroyTexture(tvictory);
	SDL_DestroyTexture(tcontinue);

	TTF_CloseFont(gSmallFont);
	TTF_CloseFont(gLargeFont);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
	return(0);
}

short int gameKeyboard(const Uint8 *currentKeyStates)
{
	int movespeed = 1; //pixels moved per frame 
	short int isedgehit = 0;

	if (currentKeyStates[SDL_SCANCODE_LCTRL] || currentKeyStates[SDL_SCANCODE_RCTRL])
	{
		if (gBoostPool > 0)
			movespeed = 3;

		gBoostPool = gBoostPool - BOOST_DOWN;
		if (gBoostPool < 0)
			gBoostPool = 0;
	}
	else
	{
		gBoostPool = gBoostPool + BOOST_RECHARGE;
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

void moveAIBox(struct AIBox *ai)
{
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
	temp.rand_direction = 0;

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
	int move = 1;

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

void DrawBox(SDL_Renderer *r, SDL_Rect *box, enum BoxColors color)
{
	//enum BoxColors {BLACK = 0, WHITE, RED, BLUE, ORANGE, GREEN}
	switch(color)
	{
	case WHITE:
		SDL_SetRenderDrawColor(r, 255, 255, 255, SDL_ALPHA_OPAQUE);
		break;
	case BLACK:
		SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
		break;
	case RED:
		SDL_SetRenderDrawColor(r, 255, 0, 0, SDL_ALPHA_OPAQUE);
		break;
	case BLUE:
		SDL_SetRenderDrawColor(r, 0, 255, 255, SDL_ALPHA_OPAQUE);
		break;
	case ORANGE:
		SDL_SetRenderDrawColor(r, 255, 165, 0, SDL_ALPHA_OPAQUE);
		break;
	case GREEN:
		SDL_SetRenderDrawColor(r, 34, 139, 34, SDL_ALPHA_OPAQUE);
		break;
	default:
		SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
		break;
	}		
	SDL_RenderFillRect(r, box);

}

SDL_Texture* makeTextTexture(SDL_Renderer *r, TTF_Font *font, const char *text, SDL_Color fg, SDL_Color bg, enum TextType tt)
{
	SDL_Surface *text_surface = NULL;
	SDL_Texture *text_texture = NULL;

	switch (tt)
	{
	case SOLID:
		if (!(text_surface = TTF_RenderText_Solid(font, text, fg)))
		{
			printf("Error creating text surface. Error: %s", TTF_GetError());
			return(NULL);
		}
		break;
	case SHADED:
		if (!(text_surface = TTF_RenderText_Shaded(font, text, fg, bg)))
		{
			printf("Error creating text surface. Error: %s", TTF_GetError());
			return(NULL);
		}
		break;
	case BLENDED:
		if (!(text_surface = TTF_RenderText_Blended(font, text, fg)))
		{
			printf("Error creating text surface. Error: %s", TTF_GetError());
			return(NULL);
		}
		break;
	default:
		if (!(text_surface = TTF_RenderText_Shaded(font, text, fg, bg)))
		{
			printf("Error creating text surface. Error: %s", TTF_GetError());
			return(NULL);
		}
		break;
	}

	text_texture = SDL_CreateTextureFromSurface(r, text_surface);
	SDL_FreeSurface(text_surface);
	text_surface = NULL;
	return(text_texture);
}

SDL_Rect* copyToSDLRect(struct AIBox *ai, SDL_Rect *sdl)
{
	sdl->x = ai->x;
	sdl->y = ai->y;
	sdl->w = ai->w;
	sdl->h = ai->h;
	return sdl;
}

void seedrnd(void)
{
	time_t t;

	srand((unsigned)time(&t));
	gRandCount = 0;
}

int rnd(int range)
{
	gRandCount++;
	if (gRandCount > STALE_RANDOM)
		seedrnd();

	return(rand() % range);
}