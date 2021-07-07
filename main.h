/*
* Box Game - main.h
* Box Game to demo SDL2 and simple AI
* Header file for all constants, includes, etc 
* By Matthew K. Daddysman
*/

# ifndef _MAIN_H_
# define _MAIN_H_

#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

//game build settings
//#define _DEBUG_BUILD_ //should debug text be shown 
#define BUILD_NUMBER 350

//constants 
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static const char *GAME_NAME = "KAESTCHENSPIEL";
static const unsigned int STALE_RANDOM = 50000;

static const int SCOREBAR_HEIGHT = 30;
static const int PLAYAREA_PADDING = 4;
static const int PLAYAREA_BORDER = 6;
static const int PLAYERBOX_WIDTH = 10;
static const int PLAYERBOX_HEIGHT = 10;
static const int MAX_BUFFER_SEARCH = 1000; //max number of loops to find a buffer away from box 
static const int MAX_VOLUME = 100;

static const char *GAME_MUSIC = "Music/Game1.mp3";
static const char *MENU_MUSIC = "Music/Menu.mp3";
static const char *WALL_SOUND = "Sounds/fail.wav";
static const char *DEATH_SOUND = "Sounds/explosion.wav";
static const char *WIN_SOUND = "Sounds/success.wav";
static const char *COUNTDOWN_SOUND = "Sounds/countdown.wav";
static const char *CLICK_SOUND = "Sounds/menuclick.wav";
static const char *GAME_FONT_FILE = "Fonts/RipeApricots.ttf";
static const char *MENU_FONT_FILE = "Fonts/EBGaramond.ttf";
static const char *SAVE_DATA = "data.dat";

static const float MAX_BOOST = 100;
static const float BOOST_DOWN_START = 1.5;
static const float BOOST_RECHARGE_START = 0.5;
static const float BOOST_DOWN_UPGRADE = (float) 0.07;
static const float BOOST_RECHARGE_UPGRADE = (float) 0.1;

static const int END_TEXT_SPACING = 2;

static const int INV_FRAMES = 60; //number of frame to be invernable after a hit - 1 sec 
static const int INV_FLASH_FRAME = 5; //on/off frames to flash to show invernablility 
static const double PLAYER_BUFFER = 50; //min distance for boxes to be away from the player to start 

static const SDL_Color TEXT_COLOR = { 255, 255, 255 };
static const SDL_Color BG_COLOR = { 0, 0, 0 };

#define MAX_AI_BOXES 50
#define NUM_MENU_BOXES 50
#define MAX_NAME 20

//custom data types 
enum ProgrameState {SHELL = 0, GAME};
enum GameState { GAMEPLAY = 0, GAMEOVER, VICTORY, COUNTDOWN, NOGAME };
enum BoxColors { BLACK = 0, WHITE, RED, BLUE, ORANGE, GREEN, YELLOW, TRANSPARENT_BLACK };
enum TextType { SOLID = 0, SHADED, BLENDED };
enum FONTS { STANDARD_SMALL = 0, STANDARD_LARGE, MENU_SMALL, MENU_LARGE, MAX_FONTS };
enum MoveDirection { NORTH = 0, SOUTH, EAST, WEST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST, numMoveDirection };
enum AIType {RANDOM = 0, SEEK, NONE};
enum MenuOptions {STARTGAME = 0, HOWTOPLAY, HIGHSCORES, OPTIONS, EXIT, numMenuOptions};
enum CurrentMenu {MAIN_MENU = 0, HOWTOPLAY_MENU, HIGHSCORES_MENU, OPTIONS_MENU, ENTERNAME_MENU};
enum OptionsMenuOptions {MASTERVOLUME = 0, MUSICVOLUME, SOUNDVOLUME, FPSONOFF, RESOLUTION, BACK_OPTIONS };
enum ResolutionOptions { r800x600 = 0, r1024x768, r1280x960, MAX_RESOLUTIONS };

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

struct Player {
	int x;
	int y;
	int w;
	int h;
	enum BoxColors color;
	bool invernable;
	bool inv_draw;
	int inv_count;
	int inv_flash_count;
};

struct HighScore {
	char Name[MAX_NAME+1];
	int level;
	unsigned long int time; 
};

//function prototypes
//found in game.c
void loadGameResources(SDL_Renderer *r);
void freeGameResources(void);
void newGame(SDL_Renderer *r); //starts a new level
void resetGame(void);  //resets back to level one after a game over, also call when loading the program 
bool gameKeyboard(SDL_Renderer *r);
void gameLogic(SDL_Renderer *r, bool isedgehit);
void updateGameTimer(SDL_Renderer *r);
void processBoxMovement();
void processCountdown(void);
void drawPlayArea(SDL_Renderer *r);
void moveAIBox(struct AIBox *ai);
void changeAIBoxCoordinates(struct AIBox *ai);
int checkAIBoxDirection(struct AIBox *ai);
bool pauseGame(SDL_Keycode keycode);
void checkEndGame(SDL_Renderer *r);
void initalizeGamePointers(void);
bool loadGameTextResources(SDL_Renderer *r);


//found in shell.c
void loadShellResources(SDL_Renderer *r);
void freeShellResources(void);
bool shellKeyboard(SDL_Event *e, SDL_Renderer *r);
void shellLogic(SDL_Renderer *r);
void drawShell(SDL_Renderer *r);
void moveShellAIBox(struct AIBox *ai);
void updateVolumes(void);
void openSaveData(void);
void writeGameDataFile(void);
void resetHighScores(void);
void openHighScoresScreen(SDL_Renderer *r);
void checkIfNewHighScore(int levelAchieved, unsigned long int time, SDL_Renderer *r);
void acceptNameInput(SDL_Event *e, SDL_Renderer *r);
bool loadTextResources(SDL_Renderer *r);
void initalizeShellPointers(void);


//found in main.c
void MoveToGame(SDL_Renderer *r);
void MoveToShell(int levelAchieved, unsigned long int time, SDL_Renderer *r);
void DrawBox(SDL_Renderer *r, SDL_Rect *box, enum BoxColors color);
SDL_Texture* makeTextTexture(SDL_Renderer *r, enum FONTS font, const char *text, SDL_Color fg, SDL_Color bg, enum TextType tt, SDL_Rect *rect);
SDL_Rect* copyToSDLRect(struct AIBox *ai, SDL_Rect *sdl);
void seedrnd(void);
int rnd(int range);
bool updateResolution(SDL_Window *win, SDL_Renderer *r);

#endif