/*
 * Box Game - main.c
 * Box Game to demo SDL2 and simple AI
 * Forked from SDL Learning repo on 9/3/2017
 * By Matthew K. Daddysman
 */

#include "main.h"

//global variables 
unsigned int gRandCount; // number of times the random number generator has been called 
TTF_Font *gSmallFont, *gLargeFont, *gMenuFont, *gMenuFontSmall;
enum ProgramState gProgramState;
bool gDrawFPS;

void MoveToGame(SDL_Renderer *r)
{
	resetGame(); //reset the global variables 
	newGame(r); //start to configure a new game 
	gProgramState = GAME;
}

void MoveToShell(int levelAchieved, unsigned long int time, SDL_Renderer *r)
{
	gProgramState = SHELL;
	checkIfNewHighScore(levelAchieved, time, r);
}

int main(int argc, char* args[])
{
	int w, h;
	int result = 0;
	char buffer[15];
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_Texture *tFPS = NULL;
	SDL_Texture *tLoading = NULL;
	SDL_Rect Loading_rect;
	SDL_Rect Clipping_rect = {
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT
	};
	
	bool quit = false;	
	bool isEdgeHit = false;
	gDrawFPS = false;
		
	int fps = 0; 
	int framecount = 0; 
	Uint32 startTick, currentTick; 
	
	SDL_Event e;

	SDL_Rect tempbox = { 0, 0, 0, 0 };
	SDL_Rect fps_rect;		

	//initalize global variables
	gSmallFont = NULL;
	gLargeFont = NULL;	
	gMenuFont = NULL;
	gMenuFontSmall = NULL;
	gProgramState = SHELL;
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

	window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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
	if (SDL_RenderSetClipRect(renderer, &Clipping_rect) < 0)
		printf("Clipping not set. Error: %s\n", SDL_GetError());
	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) < 0)
		printf("Blending not set. Error: %s\n", SDL_GetError());

	gMenuFont = TTF_OpenFont(MENU_FONT_FILE, 32);
	if (gMenuFont == NULL)
	{
		printf("Font could not be loaded Error:%s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(0);
	}
	//now that the basics are loaded display a loading message
	tLoading = makeTextTexture(renderer, gMenuFont, "Loading...", TEXT_COLOR, BG_COLOR, SOLID);
	SDL_QueryTexture(tLoading, NULL, NULL, &w, &h);
	Loading_rect.x = (SCREEN_WIDTH - w) / 2;
	Loading_rect.y = (SCREEN_HEIGHT - h) / 2;
	Loading_rect.w = w;
	Loading_rect.h = h;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //set clear color to black
	SDL_RenderClear(renderer); //clear the screen
	SDL_RenderCopy(renderer, tLoading, NULL, &Loading_rect);
	SDL_RenderPresent(renderer); //present the frame 

	gMenuFontSmall = TTF_OpenFont(MENU_FONT_FILE, 16);
	if (gMenuFont == NULL)
	{
		printf("Font could not be loaded Error:%s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(0);
	}

	gSmallFont = TTF_OpenFont(GAME_FONT_FILE, 36);
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

	gLargeFont = TTF_OpenFont(GAME_FONT_FILE, 144);
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
#ifdef _DEBUG_BUILD_
	printf("Main resources loaded\n");
#endif
	seedrnd();

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
#ifdef _DEBUG_BUILD_
	printf("Audio open\n");
#endif

	loadShellResources(renderer);
#ifdef _DEBUG_BUILD_
	printf("Shell resources loaded\n");
#endif
	loadGameResources(renderer);
#ifdef _DEBUG_BUILD_
	printf("Game resources loaded\n");
#endif
	
	sprintf_s(buffer, sizeof(buffer), "FPS: %02d", 0);
	tFPS = makeTextTexture(renderer, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
	SDL_QueryTexture(tFPS, NULL, NULL, &w, &h);
	fps_rect.x = SCREEN_WIDTH - PLAYAREA_PADDING - w - 5;
	fps_rect.y = PLAYAREA_PADDING;
	fps_rect.w = w;
	fps_rect.h = h;	
	
	framecount = 0;
	fps = 0;
	startTick = SDL_GetTicks();
	if (tLoading != NULL)
		SDL_DestroyTexture(tLoading); //clean up the loading text
	tLoading = NULL;
#ifdef _DEBUG_BUILD_
	printf("Entering game loop\n");
#endif
	while (!quit)
	{
		isEdgeHit = false;
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (gProgramState == SHELL)
					quit = shellKeyboard(&e, renderer);
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
				case SDLK_UP:
				case SDLK_DOWN:
					if (gProgramState == GAME)
						isEdgeHit = pauseGame(e.key.keysym.sym);
					break;
				case SDLK_RETURN:
					if (gProgramState == GAME)
					{
						checkEndGame(renderer);
						isEdgeHit = pauseGame(e.key.keysym.sym);
					}
					break;
				//case SDLK_f:
					//drawFPS = !drawFPS; //draw FPS used on all screens and inputs 
					//break;
				default:
					break;
				}
				break;
			case SDL_TEXTINPUT:
				acceptNameInput(&e,renderer);
				break;
			default:
				break;
			}
		}

		if (gProgramState == GAME && !isEdgeHit) //process keyboard live for game
			isEdgeHit = gameKeyboard(renderer);

		
		switch (gProgramState) //structure for processing logic
		{
		case GAME:
			gameLogic(renderer, isEdgeHit);
			break;
		case SHELL:
			shellLogic(renderer);
			break;
		default:
			break;
		}		

		//start drawing 
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //set clear color to black
		SDL_RenderClear(renderer); //clear the screen

		switch (gProgramState)
		{
		case GAME:
			drawPlayArea(renderer); //draw the playfield
			break;
		case SHELL:
			drawShell(renderer); //draw the menu 
			break;
		default:
			break;
		}

		//draw the FPS text - all game states 
		if (gDrawFPS)
		{
			if (tFPS != NULL)
				SDL_DestroyTexture(tFPS);
			sprintf_s(buffer, sizeof(buffer), "FPS: %02d", fps);
			tFPS = makeTextTexture(renderer, gSmallFont, buffer, TEXT_COLOR, BG_COLOR, SHADED);
			SDL_RenderCopy(renderer, tFPS, NULL, &fps_rect);
		}

		SDL_RenderPresent(renderer); //present the frame 

		//process clock stuff 
		currentTick = SDL_GetTicks();
		if (currentTick - startTick >= 1000) //if longer than one second
		{
			startTick = currentTick;
			fps = framecount+1;
			framecount = 0;

			switch (gProgramState)
			{
			case GAME:
				updateGameTimer(renderer);
				break;
			case SHELL:
				break;
			default:
				break;
			}
			
		}
		else
		{
			framecount++;
		}
	}

	//program is ending free all the resources 
	Mix_HaltMusic();

	freeShellResources();
#ifdef _DEBUG_BUILD_
	printf("Shell resources free\n");
#endif
	freeGameResources();
#ifdef _DEBUG_BUILD_
	printf("Game resource free\n");
#endif
	
	if (tFPS != NULL)
		SDL_DestroyTexture(tFPS);

	if (gMenuFont != NULL)
		TTF_CloseFont(gMenuFont);
	if (gMenuFont != NULL)
		TTF_CloseFont(gMenuFontSmall);
	if (gMenuFont != NULL)
		TTF_CloseFont(gSmallFont);
	if (gMenuFont != NULL)
		TTF_CloseFont(gLargeFont);
	
	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);
	if (window != NULL)
		SDL_DestroyWindow(window);
#ifdef _DEBUG_BUILD_
	printf("Main resource free\n");
#endif
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
	return(0);
}

void DrawBox(SDL_Renderer *r, SDL_Rect *box, enum BoxColors color)
{
	Uint8 a; 
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
	case TRANSPARENT_BLACK:
		a = (Uint8)(0.75 * SDL_ALPHA_OPAQUE); //25% transparent 
		SDL_SetRenderDrawColor(r, 0, 0, 0, a);
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