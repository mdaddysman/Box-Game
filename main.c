/*
 * Box Game - main.c
 * Box Game to demo SDL2 and simple AI
 * Forked from SDL Learning repo on 9/3/2017 
 * By Matthew K. Daddysman
 */

#include "main.h"

//global variables 
int gLeftEdge, gRightEdge, gTopEdge, gBottomEdge;
int gRandCount;

int main(int argc, char* args[])
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	Mix_Music *music = NULL;
	Mix_Chunk *wall = NULL;
	Mix_Chunk *death = NULL; 
	TTF_Font *font = NULL;
	TTF_Font *largefont = NULL;
	SDL_Texture *tlevel_number = NULL;
	SDL_Texture *thit_points = NULL;
	SDL_Texture *tboost = NULL;
	SDL_Texture *tgameover = NULL;

	char buffer[100];
	int quit = 0;
	int result = 0;
	short int isedgehit = 0;
	short int invernable = 0;
	short int inv_flash_count = 0;
	short int inv_draw = 0; 
	short int gameover = 0;
	int inv_count = 0; 
	int level = 1; 
	int hit_points = 3;
	int boost_pool = MAX_BOOST;
	SDL_Event e;
	enum BoxColors playercolor = WHITE;
	SDL_Rect playerbox = {
		200,
		200,
		10,
		10,
	};
	SDL_Rect tempbox = {
		200,
		200,
		10,
		10,
	};
	struct AIBox goalbox, enemybox;
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
	int movespeed = 1; //pixels moved per frame 
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Color bg_color = { 0, 0, 0 };
	SDL_Rect level_rect, hitpoints_rect, boost_rect, boostmeter_rect, boostdeplet_rect, gameover_rect;
	int w, h;

	gLeftEdge = playarea.x;
	gTopEdge = playarea.y;
	gRightEdge = playarea.x + playarea.w - playerbox.w;
	gBottomEdge = playarea.y + playarea.h - playerbox.h;

	goalbox.x = 200;
	goalbox.y = 200;
	goalbox.w = 10;
	goalbox.h = 10;
	goalbox.direction = NORTH;
	goalbox.rand_direction = 1;

	enemybox.x = 200;
	enemybox.y = 200;
	enemybox.w = 10;
	enemybox.h = 10;
	enemybox.direction = NORTH;
	enemybox.rand_direction = 1;

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

	font = TTF_OpenFont(FONT_FILE, 36);
	if (font == NULL)
	{
		printf("Font could not be loaded Error:%s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		Mix_Quit();
		SDL_Quit();
		return(0);
	}

	largefont = TTF_OpenFont(FONT_FILE, 144);
	if (font == NULL)
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
	music = Mix_LoadMUS(GAME_MUSIC);
	wall = Mix_LoadWAV(WALL_SOUND);
	death = Mix_LoadWAV(DEATH_SOUND);
	Mix_PlayMusic(music, -1);
	sprintf_s(buffer,sizeof(buffer), "Level: %d", level);
	tlevel_number = makeTextTexture(renderer, font, buffer, text_color, bg_color,SHADED);
	SDL_QueryTexture(tlevel_number, NULL, NULL, &w, &h);
	level_rect.x = PLAYAREA_PADDING;
	level_rect.y = PLAYAREA_PADDING;
	level_rect.w = w;
	level_rect.h = h;

	sprintf_s(buffer, sizeof(buffer), "Hit Points: %d", hit_points);
	thit_points = makeTextTexture(renderer, font, buffer, text_color, bg_color,SHADED);
	SDL_QueryTexture(thit_points, NULL, NULL, &w, &h);
	hitpoints_rect.x = PLAYAREA_PADDING + level_rect.w + 20;
	hitpoints_rect.y = PLAYAREA_PADDING;
	hitpoints_rect.w = w;
	hitpoints_rect.h = h;

	tboost = makeTextTexture(renderer, font, "Boost:", text_color, bg_color,SHADED);
	SDL_QueryTexture(tboost, NULL, NULL, &w, &h);
	boost_rect.x = hitpoints_rect.x + hitpoints_rect.w + 20;
	boost_rect.y = PLAYAREA_PADDING;
	boost_rect.w = w;
	boost_rect.h = h;

	boostmeter_rect.x = boost_rect.x + boost_rect.w + 5;
	boostmeter_rect.y = PLAYAREA_PADDING;
	boostmeter_rect.w = 206;
	boostmeter_rect.h = boost_rect.h - 5;

	boostdeplet_rect.y = boostmeter_rect.y + 3;
	boostdeplet_rect.h = boostmeter_rect.h - 6;
	boostdeplet_rect.x = boostmeter_rect.x; //just to initalize 
	boostdeplet_rect.w = boostmeter_rect.w; //just to initalize 

	tgameover = makeTextTexture(renderer, largefont, "GAME OVER", text_color, bg_color,BLENDED);
	SDL_QueryTexture(tgameover, NULL, NULL, &w, &h);
	gameover_rect.w = w;
	gameover_rect.h = h;
	gameover_rect.x = playarea.x + playarea.w / 2 - w / 2;
	gameover_rect.y = playarea.y + playarea.h / 2 - h / 2;
	
	playerbox.x = rnd(playarea.w-20) + playarea.x + 10; //10 pixel buffer from the edge 
	playerbox.y = rnd(playarea.h-20) + playarea.y + 10;
	
	goalbox.x = rnd(playarea.w-20) + playarea.x + 10;
	goalbox.y = rnd(playarea.h-20) + playarea.y + 10;
	goalbox.direction = rnd(numMoveDirection);

	enemybox.x = rnd(playarea.w-20) + playarea.x + 10;
	enemybox.y = rnd(playarea.h-20) + playarea.y + 10;
	enemybox.direction = rnd(numMoveDirection);

	while (quit == 0)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = 1;
		}
		isedgehit = 0;
		movespeed = 1;

		const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

		if (gameover == 0)
		{
			if (currentKeyStates[SDL_SCANCODE_LCTRL] || currentKeyStates[SDL_SCANCODE_RCTRL])
			{
				if (boost_pool > 0)
					movespeed = 3;

				boost_pool = boost_pool - BOOST_DOWN;
				if (boost_pool < 0)
					boost_pool = 0;
			}
			else
			{
				boost_pool = boost_pool + BOOST_RECHARGE;
				if (boost_pool > MAX_BOOST)
					boost_pool = MAX_BOOST;
			}

			if (currentKeyStates[SDL_SCANCODE_UP] && !currentKeyStates[SDL_SCANCODE_DOWN])
				playerbox.y = playerbox.y - movespeed;
			else if (currentKeyStates[SDL_SCANCODE_DOWN] && !currentKeyStates[SDL_SCANCODE_UP])
				playerbox.y = playerbox.y + movespeed;
			if (currentKeyStates[SDL_SCANCODE_LEFT] && !currentKeyStates[SDL_SCANCODE_RIGHT])
				playerbox.x = playerbox.x - movespeed;
			else if (currentKeyStates[SDL_SCANCODE_RIGHT] && !currentKeyStates[SDL_SCANCODE_LEFT])
				playerbox.x = playerbox.x + movespeed;

			if (playerbox.x < gLeftEdge)
			{
				playerbox.x = gLeftEdge;
				isedgehit = 1;
			}
			else if (playerbox.x > gRightEdge)
			{
				playerbox.x = gRightEdge;
				isedgehit = 1;
			}

			if (playerbox.y < gTopEdge)
			{
				playerbox.y = gTopEdge;
				isedgehit = 1;
			}
			else if (playerbox.y > gBottomEdge)
			{
				playerbox.y = gBottomEdge;
				isedgehit = 1;
			}
		}
		else
		{
			if (currentKeyStates[SDL_SCANCODE_RETURN])
				quit = 1;
		}

		if (isedgehit == 1 && invernable == 0)
		{
			if (hit_points <= 0)
			{
				gameover = 1;
				Mix_HaltMusic();
				Mix_PlayChannel(-1, death, 0);
				thit_points = makeTextTexture(renderer, font, "Hit Points:  ", text_color, bg_color, SHADED);
				SDL_QueryTexture(thit_points, NULL, NULL, &w, &h);
				hitpoints_rect.w = w;
				hitpoints_rect.h = h;
			}
			else
			{
				Mix_PlayChannel(-1, wall, 0);
				playercolor = RED;
				hit_points--;
				invernable = 1;
				inv_count = 0;
				inv_draw = 1;
				inv_flash_count = 0;
				sprintf_s(buffer, sizeof(buffer), "Hit Points: %d", hit_points);
				thit_points = makeTextTexture(renderer, font, buffer, text_color, bg_color,SHADED);
				SDL_QueryTexture(thit_points, NULL, NULL, &w, &h);
				hitpoints_rect.w = w;
				hitpoints_rect.h = h;
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
		moveAIBox(&goalbox);
		moveAIBox(&enemybox);

		//clear the screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		//draw the header
		SDL_RenderCopy(renderer, tlevel_number, NULL, &level_rect);
		SDL_RenderCopy(renderer, thit_points, NULL, &hitpoints_rect);
		SDL_RenderCopy(renderer, tboost, NULL, &boost_rect);
		DrawBox(renderer, &boostmeter_rect, GREEN);
		if (boost_pool < MAX_BOOST)
		{
			boostdeplet_rect.w = MAX_BOOST-boost_pool;  
			boostdeplet_rect.x = boostmeter_rect.x+boostmeter_rect.w-3-boostdeplet_rect.w; //just to initalize 
			DrawBox(renderer, &boostdeplet_rect, BLACK);			
		}
		//draw playfield
		DrawBox(renderer, &border, playercolor);
		DrawBox(renderer, &playarea, BLACK);
		//draw the dynamic boxes
		DrawBox(renderer, copyToSDLRect(&goalbox, &tempbox), BLUE);
		DrawBox(renderer, copyToSDLRect(&enemybox, &tempbox), ORANGE);
		if (invernable == 1 && inv_draw == 1)
			DrawBox(renderer, &playerbox, playercolor);
		else if (invernable == 0 && gameover == 0)
			DrawBox(renderer, &playerbox, playercolor);

		if (gameover == 1)
			SDL_RenderCopy(renderer, tgameover, NULL, &gameover_rect); //if the game is over draw the Game Over text 
		SDL_RenderPresent(renderer); //present the frame 
	}

	Mix_HaltMusic();
	Mix_FreeMusic(music);
	Mix_FreeChunk(wall);
	Mix_FreeChunk(death);

	SDL_DestroyTexture(tlevel_number);
	SDL_DestroyTexture(thit_points);
	SDL_DestroyTexture(tboost);
	SDL_DestroyTexture(tgameover);	

	TTF_CloseFont(font);
	TTF_CloseFont(largefont);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
	return(0);
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