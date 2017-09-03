/*
 * Box Game - main.c
 * Box Game to demo SDL2 and simple AI
 * Forked from SDL Learning repo on 9/3/2017 
 * By Matthew K. Daddysman
 */

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int SCOREBAR_HEIGHT = 30;
const int PLAYAREA_PADDING = 4; 
const int PLAYAREA_BORDER = 6; 

static const char *GAME_MUSIC = "Music/Game1.mp3";
static const char *WALL_SOUND = "Sounds/fail.wav";
static const char *DEATH_SOUND = "Sounds/explosion.wav";
static const char *FONT_FILE = "Fonts/RipeApricots.ttf";

enum BoxColors {BLACK = 0, WHITE, RED, BLUE, ORANGE, GREEN};
enum TextType {SOLID = 0, SHADED, BLENDED};

const int MAX_BOOST = 200; 
const int BOOST_DOWN = 3;
const int BOOST_RECHARGE = 1;
const int INV_FRAMES = 60; //number of frame to be invernable after a hit - 1 sec 
const int INV_FLASH_FRAME = 5; //on/off frames to flash to show invernablility 

void DrawBox(SDL_Renderer *r, SDL_Rect *box, enum BoxColors color); 
SDL_Texture* makeTextTexture(SDL_Renderer *r, TTF_Font *font, const char *text, SDL_Color fg, SDL_Color bg, enum TextType tt);

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
	time_t t;
	SDL_Rect playerbox = {
		200,
		200,
		10,
		10,
	};
	SDL_Rect goalbox = {
		200,
		200,
		10,
		10,
	};
	SDL_Rect enemybox = {
		200,
		200,
		10,
		10,
	};
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
	int left_edge, right_edge, top_edge, bottom_edge;
	int movespeed = 1; //pixels moved per frame 
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Color bg_color = { 0, 0, 0 };
	SDL_Rect level_rect, hitpoints_rect, boost_rect, boostmeter_rect, boostdeplet_rect, gameover_rect;
	int w, h;
	left_edge = playarea.x;
	top_edge = playarea.y;
	right_edge = playarea.x + playarea.w - playerbox.w;
	bottom_edge = playarea.y + playarea.h - playerbox.h;

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

	srand((unsigned)time(&t));

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
	
	playerbox.x = rand() % playarea.w + playarea.x;
	playerbox.y = rand() % playarea.h + playarea.y;
	
	goalbox.x = rand() % playarea.w + playarea.x;
	goalbox.y = rand() % playarea.h + playarea.y;

	enemybox.x = rand() % playarea.w + playarea.x;
	enemybox.y = rand() % playarea.h + playarea.y;

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

			if (playerbox.x < left_edge)
			{
				playerbox.x = left_edge;
				isedgehit = 1;
			}
			else if (playerbox.x > right_edge)
			{
				playerbox.x = right_edge;
				isedgehit = 1;
			}

			if (playerbox.y < top_edge)
			{
				playerbox.y = top_edge;
				isedgehit = 1;
			}
			else if (playerbox.y > bottom_edge)
			{
				playerbox.y = bottom_edge;
				isedgehit = 1;
			}
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
		DrawBox(renderer, &goalbox, BLUE);
		DrawBox(renderer, &enemybox, ORANGE);
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