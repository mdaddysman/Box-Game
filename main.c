/*
 * Box Game - main.c
 * Box Game to demo SDL2 and simple AI
 * Forked from SDL Learning repo on 9/3/2017
 * By Matthew K. Daddysman
 */

#include "main.h"

//global variables 
int gRandCount; // number of times the random number generator has been called 
TTF_Font *gSmallFont, *gLargeFont;

//game globals to extern 
extern int gLeftEdge, gRightEdge, gTopEdge, gBottomEdge; //define the edges of the playfield for bounds checking 
extern short int gIsCountdown, gCountdownSecCount; //is the countdown running? 
extern SDL_Rect gPlayerBox, gPlayarea;

//remove these externs with more work
extern struct AIBox gGoalBox;
extern int gLevel, gHitPoints, gNumAIs;
extern float gBoostPool, gBoostDown, gBoostRecharge; //the Boost pool in percent. float for fraction and how fast it charges and recharges
extern SDL_Texture *gtHitPoints;
extern SDL_Rect gHitPoints_rect;

int main(int argc, char* args[])
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	Mix_Chunk *wall = NULL;
	Mix_Chunk *death = NULL;
	Mix_Chunk *win = NULL;
	
	SDL_Texture *tFPS = NULL;

	SDL_Texture *tgameover = NULL;
	SDL_Texture *tvictory = NULL;
	SDL_Texture *tcontinue = NULL;

	SDL_Texture *tmoreAI = NULL;
	SDL_Texture *thardGoal = NULL;
	SDL_Texture *trecover = NULL;
	SDL_Texture *tfasterAI = NULL;
	SDL_Texture *tfasterrecharge = NULL;
	SDL_Texture *yellowbox = NULL;

	SDL_Rect upgradeRects[5];
	SDL_Texture *upgradeTextures[5];

	SDL_Rect countdownRects[3];
	SDL_Texture *countdownTextures[3];

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
	int fps = 0; 
	int framecount = 0; 
	Uint32 startTick, currentTick; 
	
	SDL_Event e;
	enum BoxColors playercolor = WHITE;

	SDL_Rect tempbox = { 0, 0, 0, 0 };

	SDL_Rect gameover_rect, victory_rect, continue_rect, fps_rect;
	int w, h, i;
	
	short int numUpgrades = 0;
	const int END_TEXT_SPACING = 2;

	//initalize global variables
	gSmallFont = NULL;
	gLargeFont = NULL;	
	//end initalize global variables

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! Error: %s\n", SDL_GetError());
		SDL_Quit();
		return(0);
	}

	if (MIX_INIT_MP3 != (result = Mix_Init(MIX_INIT_MP3)))
	{
		printf("SDL Mixer could not initialize! Code: %d Error: %s\n", result, SDL_GetError());
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

	window = SDL_CreateWindow("Star Box", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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

	loadGameResources(renderer);

	wall = Mix_LoadWAV(WALL_SOUND);
	death = Mix_LoadWAV(DEATH_SOUND);
	win = Mix_LoadWAV(WIN_SOUND);
	
	sprintf_s(buffer, sizeof(buffer), "FPS: %02d", 0);
	tFPS = makeTextTexture(renderer, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
	SDL_QueryTexture(tFPS, NULL, NULL, &w, &h);
	fps_rect.x = SCREEN_WIDTH - PLAYAREA_PADDING - w - 5;
	fps_rect.y = PLAYAREA_PADDING;
	fps_rect.w = w;
	fps_rect.h = h;

	tgameover = makeTextTexture(renderer, gLargeFont, "GAME OVER", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(tgameover, NULL, NULL, &w, &h);
	gameover_rect.w = w;
	gameover_rect.h = h;
	gameover_rect.x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
	gameover_rect.y = gPlayarea.y + gPlayarea.h / 2 - h / 2;

	tvictory = makeTextTexture(renderer, gLargeFont, "Level Complete!", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(tvictory, NULL, NULL, &w, &h);
	victory_rect.w = w;
	victory_rect.h = h;
	victory_rect.x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
	victory_rect.y = gPlayarea.y + gPlayarea.h / 2 - h / 2;

	tcontinue = makeTextTexture(renderer, gSmallFont, "Press ENTER to continue", TEXT_COLOR, BG_COLOR, BLENDED);
	SDL_QueryTexture(tcontinue, NULL, NULL, &w, &h);
	continue_rect.w = w;
	continue_rect.h = h;
	continue_rect.x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
	continue_rect.y = gPlayarea.y + gPlayarea.h / 2 - h / 2;

	tmoreAI = makeTextTexture(renderer, gSmallFont, "You will now face more opposition.", TEXT_COLOR, BG_COLOR, BLENDED);
	trecover = makeTextTexture(renderer, gSmallFont, "Good Work! You gain 1 hit point and a longer boost!", TEXT_COLOR, BG_COLOR, BLENDED);
	thardGoal = makeTextTexture(renderer, gSmallFont, "Your goal will now behave more erratically.", TEXT_COLOR, BG_COLOR, BLENDED);
	tfasterAI = makeTextTexture(renderer, gSmallFont, "Some of your opposition will now move faster.", TEXT_COLOR, BG_COLOR, BLENDED);
	tfasterrecharge = makeTextTexture(renderer, gSmallFont, "Your boost will also recharge faster.", TEXT_COLOR, BG_COLOR, BLENDED);
	yellowbox = makeTextTexture(renderer, gSmallFont, "A new challenge! The yellow box will home in on your position.", TEXT_COLOR, BG_COLOR, BLENDED);

	countdownTextures[0] = makeTextTexture(renderer, gLargeFont, "3", TEXT_COLOR, BG_COLOR, BLENDED);
	countdownTextures[1] = makeTextTexture(renderer, gLargeFont, "2", TEXT_COLOR, BG_COLOR, BLENDED);
	countdownTextures[2] = makeTextTexture(renderer, gLargeFont, "1", TEXT_COLOR, BG_COLOR, BLENDED);
	for (i = 0; i < 3; i++)
	{
		SDL_QueryTexture(countdownTextures[i], NULL, NULL, &w, &h);
		countdownRects[i].w = w;
		countdownRects[i].h = h;
		countdownRects[i].x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
		countdownRects[i].y = gPlayarea.y + gPlayarea.h / 2 - h / 2;
	}

	upgradeTextures[0] = tmoreAI; upgradeTextures[1] = tfasterAI; upgradeTextures[2] = thardGoal; //avoid NULL pointers 
	upgradeTextures[3] = trecover; upgradeTextures[4] = tfasterrecharge;

	resetGame(); //reset the global variables 
	newGame(renderer); //start to configure a new game 

	framecount = 0;
	fps = 0;
	startTick = SDL_GetTicks();

	while (quit == 0)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = 1;
		}


		const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

		if (gameover == 0 && victory == 0 && gIsCountdown == 0)
		{
			isedgehit = gameKeyboard(currentKeyStates);
		}
		else if (gameover == 1)
		{
			if (currentKeyStates[SDL_SCANCODE_RETURN])
			{
				gameover = 0;
				invernable = 0;
				isedgehit = 0;
				resetGame(); //game is over so reset back to starting values
				newGame(renderer); //then start a new game 
				continue;
			}

		}
		else if (victory == 1)
		{
			if (currentKeyStates[SDL_SCANCODE_RETURN])
			{
				victory = 0;
				invernable = 0;
				isedgehit = 0;
				newGame(renderer);
				continue;
			}

		}

		//check for collision 
		if (invernable == 0)
		{
			isedgehit = checkEnemyCollision();
		}

		if (invernable == 0 && victory == 0 && gameover == 0 &&
			SDL_HasIntersection(&gPlayerBox, copyToSDLRect(&gGoalBox, &tempbox)) == SDL_TRUE)
		{
			victory = 1;
			Mix_HaltMusic();
			Mix_PlayChannel(-1, win, 0);

			if (gLevel < 100)
			{
				gLevel++;
				numUpgrades = 0;
				if ((gLevel % 10) == 0)
				{
					gBoostRecharge = gBoostRecharge + BOOST_RECHARGE_UPGRADE;
				}
				if ((gLevel % 5) == 0)
				{
					gHitPoints++;
					gBoostDown = gBoostDown - BOOST_DOWN_UPGRADE;
					upgradeTextures[numUpgrades] = trecover;
					numUpgrades++;
					if ((gLevel % 10) == 0)
					{
						gBoostRecharge = gBoostRecharge + BOOST_RECHARGE_UPGRADE;
						upgradeTextures[numUpgrades] = tfasterrecharge;
						numUpgrades++;
					}
				}
				if ((gLevel % 2) == 0 && gNumAIs < MAX_AI_BOXES - 1)
				{
					gNumAIs++;
					upgradeTextures[numUpgrades] = tmoreAI;
					numUpgrades++;
				}
				if ((gLevel % 3) == 0)
				{
					gGoalBox.rand_direction = gGoalBox.rand_direction + 1;
					upgradeTextures[numUpgrades] = thardGoal;
					numUpgrades++;
				}
				if ((gLevel % 10) == 2 && gLevel > 10)
				{
					upgradeTextures[numUpgrades] = tfasterAI;
					numUpgrades++;
				}
				if (gLevel == 10)
				{
					upgradeTextures[numUpgrades] = yellowbox;
					numUpgrades++;
				}
				for (i = 0; i < numUpgrades; i++)
				{
					SDL_QueryTexture(upgradeTextures[i], NULL, NULL, &w, &h);
					upgradeRects[i].w = w;
					upgradeRects[i].h = h;
					upgradeRects[i].x = gPlayarea.x + gPlayarea.w / 2 - w / 2;
					upgradeRects[i].y = 0; //set later
				}
			}
		}

		if (isedgehit == 1 && invernable == 0 && gameover == 0 && victory == 0)
		{
			if (gHitPoints <= 0)
			{
				gameover = 1;
				Mix_HaltMusic();
				Mix_PlayChannel(-1, death, 0);

				if (gtHitPoints != NULL)
					SDL_DestroyTexture(gtHitPoints);
				gtHitPoints = makeTextTexture(renderer, gSmallFont, "Hit Points:  ", TEXT_COLOR, BG_COLOR, SHADED);
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

				if (gtHitPoints != NULL)
					SDL_DestroyTexture(gtHitPoints);
				sprintf_s(buffer, sizeof(buffer), "Hit Points: %d", gHitPoints);
				gtHitPoints = makeTextTexture(renderer, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
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

		processBoxMovement(victory); //process box movement
		

		//check countdown
		if (gIsCountdown == 1)
			processCountdown();

		//start drawing 
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //set clear color to black
		SDL_RenderClear(renderer); //clear the screen
		//draw the FPS text 
		if (tFPS != NULL)
			SDL_DestroyTexture(tFPS);
		sprintf_s(buffer, sizeof(buffer), "FPS: %02d", fps);
		tFPS = makeTextTexture(renderer, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
		SDL_RenderCopy(renderer, tFPS, NULL, &fps_rect);

		drawPlayArea(renderer, playercolor,victory); //draw the header
	
		//if the logic is right draw the player box 
		if (invernable == 1 && inv_draw == 1)
			DrawBox(renderer, &gPlayerBox, playercolor);
		else if (invernable == 0 && gameover == 0)
			DrawBox(renderer, &gPlayerBox, playercolor);

		if (gameover == 1) //draw game over text 
		{
			SDL_RenderCopy(renderer, tgameover, NULL, &gameover_rect); //if the game is over draw the Game Over text 
			continue_rect.y = gameover_rect.y + gameover_rect.h + END_TEXT_SPACING;
			SDL_RenderCopy(renderer, tcontinue, NULL, &continue_rect);
		}
		if (victory == 1) //draw win text and any notifications 
		{
			SDL_RenderCopy(renderer, tvictory, NULL, &victory_rect); //if the game is won draw the Victory text 
			continue_rect.y = victory_rect.y + victory_rect.h + END_TEXT_SPACING;
			SDL_RenderCopy(renderer, tcontinue, NULL, &continue_rect);
			if (numUpgrades > 0)
			{
				upgradeRects[0].y = continue_rect.y + continue_rect.h + END_TEXT_SPACING;
				SDL_RenderCopy(renderer, upgradeTextures[0], NULL, &upgradeRects[0]);
				for (i = 1; i < numUpgrades; i++)
				{
					upgradeRects[i].y = upgradeRects[i - 1].y + upgradeRects[i - 1].h + END_TEXT_SPACING;
					SDL_RenderCopy(renderer, upgradeTextures[i], NULL, &upgradeRects[i]);
				}
			}
		}
		//draw the countdown if in countdown
		if (gIsCountdown)
			SDL_RenderCopy(renderer, countdownTextures[gCountdownSecCount], NULL, &countdownRects[gCountdownSecCount]);

		SDL_RenderPresent(renderer); //present the frame 

		//process clock stuff 
		currentTick = SDL_GetTicks();
		if (currentTick - startTick >= 1000) //if longer than one second
		{
			startTick = currentTick;
			fps = framecount+1;
			framecount = 0;

			//update the timer if the game is still running
			if (victory == 0 && gameover == 0 && gIsCountdown == 0) 
			{
				updateGameTimer(renderer);
			}
		}
		else
		{
			framecount++;

		}
	}

	//program is ending free all the resources 
	Mix_HaltMusic();

	freeGameResources();
	
	Mix_FreeChunk(wall);
	Mix_FreeChunk(death);
	Mix_FreeChunk(win);
	
	SDL_DestroyTexture(tFPS);
	SDL_DestroyTexture(tgameover);
	SDL_DestroyTexture(tvictory);
	SDL_DestroyTexture(tcontinue);
	for (i = 0; i < 3; i++)
		SDL_DestroyTexture(countdownTextures[i]);


	SDL_DestroyTexture(tmoreAI);
	SDL_DestroyTexture(trecover);
	SDL_DestroyTexture(thardGoal);
	SDL_DestroyTexture(tfasterAI);
	SDL_DestroyTexture(tfasterrecharge);
	SDL_DestroyTexture(yellowbox);

	TTF_CloseFont(gSmallFont);
	TTF_CloseFont(gLargeFont);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
	return(0);
}

void DrawBox(SDL_Renderer *r, SDL_Rect *box, enum BoxColors color)
{
	//enum BoxColors {BLACK = 0, WHITE, RED, BLUE, ORANGE, GREEN}
	switch (color)
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
	case YELLOW:
		SDL_SetRenderDrawColor(r, 255, 255, 0, SDL_ALPHA_OPAQUE);
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