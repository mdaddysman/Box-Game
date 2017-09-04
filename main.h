/*
* Box Game - main.h
* Box Game to demo SDL2 and simple AI
* Header file for all constants, includes, etc 
* By Matthew K. Daddysman
*/

# ifndef _MAIN_H_
# define _MAIN_H_

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//constants 
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int STALE_RANDOM = 50000;

const int SCOREBAR_HEIGHT = 30;
const int PLAYAREA_PADDING = 4;
const int PLAYAREA_BORDER = 6;

static const char *GAME_MUSIC = "Music/Game1.mp3";
static const char *WALL_SOUND = "Sounds/fail.wav";
static const char *DEATH_SOUND = "Sounds/explosion.wav";
static const char *FONT_FILE = "Fonts/RipeApricots.ttf";

const int MAX_BOOST = 200;
const int BOOST_DOWN = 3;
const int BOOST_RECHARGE = 1;
const int INV_FRAMES = 60; //number of frame to be invernable after a hit - 1 sec 
const int INV_FLASH_FRAME = 5; //on/off frames to flash to show invernablility 

//custom data types 
enum BoxColors { BLACK = 0, WHITE, RED, BLUE, ORANGE, GREEN };
enum TextType { SOLID = 0, SHADED, BLENDED };
enum MoveDirection { NORTH = 0, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST };
const int numMoveDirection = 8;

struct AIBox {
	int x;
	int y;
	int w;
	int h;
	enum MoveDirection direction;
	int rand_direction; //chance of randomizing the direction, out of 100 
};

//function prototypes 
void DrawBox(SDL_Renderer *r, SDL_Rect *box, enum BoxColors color);
SDL_Texture* makeTextTexture(SDL_Renderer *r, TTF_Font *font, const char *text, SDL_Color fg, SDL_Color bg, enum TextType tt);
SDL_Rect* copyToSDLRect(struct AIBox *ai, SDL_Rect *sdl);
void moveAIBox(struct AIBox *ai);
void changeAIBoxCoordinates(struct AIBox *ai);
int checkAIBoxDirection(struct AIBox *ai);
void seedrnd(void);
int rnd(int range);

#endif