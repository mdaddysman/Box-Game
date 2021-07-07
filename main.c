/*
 * Box Game - main.c
 * Box Game to demo SDL2 and simple AI
 * Forked from SDL Learning repo on 9/3/2017
 * By Matthew K. Daddysman
 */

#include "main.h"

//global variables 
unsigned int gRandCount; // number of times the random number generator has been called 
//TTF_Font *gSmallFont[MAX_FONTS], *gLargeFont[MAX_FONTS], *gMenuFont[MAX_FONTS], *gMenuFontSmall[MAX_FONTS];
TTF_Font *gFontList[MAX_FONTS][MAX_RESOLUTIONS];
enum ProgramState gProgramState;
bool gDrawFPS;
bool gUpdateResolution;

extern enum ResolutionOptions gResolutionOptions;

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
	int w, h, i, j;
	int result = 0;
	char buffer[15];
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Surface *icon = NULL;

	SDL_Texture *tFPS = NULL;
	SDL_Surface *sLoading = NULL;
	SDL_Texture *tLoading = NULL;
	SDL_Rect Loading_rect;
	SDL_Rect Clipping_rect = {
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT
	};
	TTF_Font *TempFont = NULL;
	initalizeShellPointers();
	initalizeGamePointers();
	
	bool quit = false;	
	bool isEdgeHit = false;
	gDrawFPS = false;
	gUpdateResolution = false;
		
	int fps = 0; 
	int framecount = 0; 
	Uint32 startTick, currentTick; 
	
	SDL_Event e;

	SDL_Rect tempbox = { 0, 0, 0, 0 };
	SDL_Rect fps_rect;		

	//initalize global variables
	for (i = 0; i < MAX_FONTS; i++)
		for (j = 0; j < MAX_RESOLUTIONS; j++)
			gFontList[i][j] = NULL;

	gProgramState = SHELL;
	//end initalize global variables

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! Error: %s\n", SDL_GetError());
		SDL_Quit();
		return(-1);
	}

	if (MIX_INIT_MP3 != (result = Mix_Init(MIX_INIT_MP3)))
	{
		printf("SDL Mixer could not initialize! Code: %d Error: %s\n", result, SDL_GetError());
		SDL_Quit();
		return(-2);
	}

	if (TTF_Init() < 0)
	{
		printf("SDL TTF could not initialize! Error: %s\n", SDL_GetError());
		Mix_Quit();
		SDL_Quit();
		return(-3);
	}

	window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		printf("Window could not be created! Error: %s\n", SDL_GetError());
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(-4);
	}
	icon = SDL_CreateRGBSurface(0, 32, 32, 16, 0, 0, 0, 0); //make a blue icon
	SDL_FillRect(icon, NULL, SDL_MapRGB(icon->format, 0, 255, 255)); //fill with color
	SDL_SetWindowIcon(window, icon); //attach it to the window
	SDL_FreeSurface(icon); //free the surface and set the pointer to NULL 
	icon = NULL;
#ifdef _DEBUG_BUILD_
	printf("Window Success!\n");
#endif
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(-5);
	}
	if (SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT))
		printf("Renderer Logical Size not set. Error: %s\n", SDL_GetError());
	if (SDL_RenderSetClipRect(renderer, &Clipping_rect) < 0)
		printf("Clipping not set. Error: %s\n", SDL_GetError());
	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) < 0)
		printf("Blending not set. Error: %s\n", SDL_GetError());
#ifdef _DEBUG_BUILD_
	printf("Renderer Success!\n");
#endif
	TempFont = TTF_OpenFont(MENU_FONT_FILE, 32);
	//now that the basics are loaded display a loading message
	sLoading = TTF_RenderText_Solid(TempFont, "Loading...", TEXT_COLOR);
	tLoading = SDL_CreateTextureFromSurface(renderer, sLoading);
	SDL_FreeSurface(sLoading);
	sLoading = NULL;
	SDL_QueryTexture(tLoading, NULL, NULL, &w, &h);
	Loading_rect.x = (SCREEN_WIDTH - w) / 2;
	Loading_rect.y = (SCREEN_HEIGHT - h) / 2;
	Loading_rect.w = w;
	Loading_rect.h = h;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); //set clear color to black
	SDL_RenderClear(renderer); //clear the screen
	SDL_RenderCopy(renderer, tLoading, NULL, &Loading_rect);
	SDL_RenderPresent(renderer); //present the frame 

	//load the font maxtrix 
	gFontList[0][0] = TTF_OpenFont(GAME_FONT_FILE, 36);
	gFontList[1][0] = TTF_OpenFont(GAME_FONT_FILE, 144);
	gFontList[2][0] = TTF_OpenFont(MENU_FONT_FILE, 16);
	gFontList[3][0] = TTF_OpenFont(MENU_FONT_FILE, 32);

	gFontList[0][1] = TTF_OpenFont(GAME_FONT_FILE, (int)(36 * 1.28));
	gFontList[1][1] = TTF_OpenFont(GAME_FONT_FILE, (int)(144 * 1.28));
	gFontList[2][1] = TTF_OpenFont(MENU_FONT_FILE, (int)(16 * 1.28));
	gFontList[3][1] = TTF_OpenFont(MENU_FONT_FILE, (int)(32 * 1.28));

	gFontList[0][2] = TTF_OpenFont(GAME_FONT_FILE, (int)(36 * 1.6));
	gFontList[1][2] = TTF_OpenFont(GAME_FONT_FILE, (int)(144 * 1.6));
	gFontList[2][2] = TTF_OpenFont(MENU_FONT_FILE, (int)(16 * 1.6));
	gFontList[3][2] = TTF_OpenFont(MENU_FONT_FILE, (int)(32 * 1.6));
		
#ifdef _DEBUG_BUILD_
	printf("Main resources loaded\n");
#endif
	seedrnd();

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
#ifdef _DEBUG_BUILD_
	printf("Audio open\n");
#endif
	loadTextResources(renderer);
#ifdef _DEBUG_BUILD_
	printf("Text resources loaded\n");
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
	tFPS = makeTextTexture(renderer, STANDARD_SMALL, buffer, TEXT_COLOR, BG_COLOR, SHADED, &fps_rect);
	fps_rect.x = SCREEN_WIDTH - PLAYAREA_PADDING - fps_rect.w - 5;
	fps_rect.y = PLAYAREA_PADDING;
	
	framecount = 0;
	fps = 0;
	startTick = SDL_GetTicks();
	if (tLoading != NULL)
		SDL_DestroyTexture(tLoading); //clean up the loading text
	tLoading = NULL;
	if (TempFont != NULL)
		TTF_CloseFont(TempFont);

#ifdef _DEBUG_BUILD_
	printf("Entering game loop\n");
#endif
	while (!quit)
	{
		isEdgeHit = false;
		if (gUpdateResolution)
			updateResolution(window,renderer);

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
				case SDLK_w:
				case SDLK_s:
					if (gProgramState == GAME)
						isEdgeHit = pauseGame(e.key.keysym.sym);
					break;
				case SDLK_RETURN:
				case SDLK_SPACE:
					if (gProgramState == GAME)
					{
						checkEndGame(renderer);
						isEdgeHit = pauseGame(e.key.keysym.sym);
					}
					break;
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
			tFPS = makeTextTexture(renderer, STANDARD_SMALL, buffer, TEXT_COLOR, BG_COLOR, SHADED, &fps_rect);
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

	for (i = 0; i < MAX_FONTS; i++)
	{
		for (j = 0; j < MAX_RESOLUTIONS; j++)
		{
			if (gFontList[i][j] != NULL)
				TTF_CloseFont(gFontList[i][j]);
			gFontList[i][j] = NULL;
		}
	}			
	
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

SDL_Texture* makeTextTexture(SDL_Renderer *r, enum FONTS efont, const char *text, SDL_Color fg, SDL_Color bg, enum TextType tt, SDL_Rect *rect)
{
	SDL_Surface *text_surface = NULL;
	SDL_Texture *text_texture = NULL;
	TTF_Font *font, *basefont; 
	int w, h;

	font = gFontList[efont][gResolutionOptions];
	basefont = gFontList[efont][0];
	//tt = SOLID; //ignore the input choice, solid looks better! 

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
	TTF_SizeText(basefont, text, &w, &h);
	rect->w = w;
	rect->h = h;
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

bool updateResolution(SDL_Window *win, SDL_Renderer *r)
{
	int w, h; 

	switch (gResolutionOptions)
	{
	case r800x600:
		w = 800; h = 600;
		break;
	case r1024x768:
		w = 1024; h = 768;
		break;
	case r1280x960:
		w = 1280; h = 960;
		break;
	default:
		break;
	}

	SDL_SetWindowSize(win, w, h);
	SDL_Rect Clipping_rect = {
		0, 0,
		w, h
	};
	SDL_RenderSetClipRect(r, &Clipping_rect);
	SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	loadTextResources(r);
	gUpdateResolution = false;
	return true;
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

