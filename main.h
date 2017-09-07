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
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static const int STALE_RANDOM = 50000;

static const int SCOREBAR_HEIGHT = 30;
static const int PLAYAREA_PADDING = 4;
static const int PLAYAREA_BORDER = 6;
static const int PLAYERBOX_WIDTH = 10;
static const int PLAYERBOX_HEIGHT = 10;

static const char *GAME_MUSIC = "Music/Game1.mp3";
static const char *WALL_SOUND = "Sounds/fail.wav";
static const char *DEATH_SOUND = "Sounds/explosion.wav";
static const char *WIN_SOUND = "Sounds/success.wav";
static const char *COUNTDOWN_SOUND = "Sounds/countdown.wav";
static const char *FONT_FILE = "Fonts/RipeApricots.ttf";

static const float MAX_BOOST = 100;
static const float BOOST_DOWN_START = 1.5;
static const float BOOST_RECHARGE_START = 0.5;
static const float BOOST_DOWN_UPGRADE = (float) 0.07;
static const float BOOST_RECHARGE_UPGRADE = (float) 0.1;
static const int INV_FRAMES = 60; //number of frame to be invernable after a hit - 1 sec 
static const int INV_FLASH_FRAME = 5; //on/off frames to flash to show invernablility 
static const double PLAYER_BUFFER = 50; //min distance for boxes to be away from the player to start 

static const SDL_Color TEXT_COLOR = { 255, 255, 255 };
static const SDL_Color BG_COLOR = { 0, 0, 0 };

#define MAX_AI_BOXES 50

//custom data types 
enum BoxColors { BLACK = 0, WHITE, RED, BLUE, ORANGE, GREEN, YELLOW };
enum TextType { SOLID = 0, SHADED, BLENDED };
enum MoveDirection { NORTH = 0, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST, numMoveDirection };
enum AIType {RANDOM = 0, SEEK, NONE};

struct AIBox {
	int x;
	int y;
	int w;
	int h;
	enum MoveDirection direction;
	int rand_direction; //chance of randomizing the direction, out of 100 
	int speed;
	enum BoxColors color;
	enum AIType aitype;
};

//function prototypes
//found in game.c
void loadGameResources(SDL_Renderer *r);
void freeGameResources(void);
void newGame(SDL_Renderer *r); //starts a new level
void resetGame(void);  //resets back to level one after a game over, also call when loading the program 
short int gameKeyboard(const Uint8 *currentKeyStates);
void updateGameTimer(SDL_Renderer *r);
short int checkEnemyCollision(void);
void processBoxMovement(short int isVictory);
void processCountdown(void);
void drawPlayArea(SDL_Renderer *r, enum BoxColors playercolor, short int isVictory);
void moveAIBox(struct AIBox *ai);
void changeAIBoxCoordinates(struct AIBox *ai);
int checkAIBoxDirection(struct AIBox *ai);

//found in main.c
void DrawBox(SDL_Renderer *r, SDL_Rect *box, enum BoxColors color);
SDL_Texture* makeTextTexture(SDL_Renderer *r, TTF_Font *font, const char *text, SDL_Color fg, SDL_Color bg, enum TextType tt);
SDL_Rect* copyToSDLRect(struct AIBox *ai, SDL_Rect *sdl);
void seedrnd(void);
int rnd(int range);

#endif