#include <stdint.h>
#include "raylib.h"
#include "config.h"
#include "sprites.h"
#include "handler.h"
#include "cursor.h"

#ifndef GAME_H_
#define GAME_H_

// Render target resolution
#define VIRTUAL_WIDTH	(1920 * 0.5f)	
#define VIRTUAL_HEIGHT	(1080 * 0.5f)
//#define VIRTUAL_WIDTH	(1920)	
//#define VIRTUAL_HEIGHT	(1080)

#define SPR_POOL &game->sprite_loader.spr_pool

// Game flags
#define GAME_QUIT_REQUEST   0x01

enum GAME_STATES {
	GAME_TITLE,
	GAME_MAIN,
	GAME_END,
	GAME_OPTIONS
};

enum INPUT_METHODS {
	KEYBOARD,
	GAMEPAD
};

typedef struct {
	Handler handler;

	Config conf;
	Camera2D cam;

	Cursor cursor;

	Rectangle render_src_rec;
	Rectangle render_dest_rec;

	uint8_t flags; 
	uint8_t state;
} Game;

void GameInit(Game *game);
void GameRenderInit(Game *game);
void GameContentInit(Game *game);

void GameUpdate(Game *game);

void GameDrawToBuffer(Game *game, uint8_t flags);
void GameDrawToWindow(Game *game);

void GameClose(Game *game);

void TitleUpdate(Game *game, float delta_time);
void TitleDraw(Game *game, uint8_t flags);

void MainUpdate(Game *game, float delta_time);
void MainDraw(Game *game, uint8_t flags);

void OverScreenUpdate(Game *game, float delta_time);
void OverScreenDraw(Game *game, uint8_t flags);

void OptionsScreenUpdate(Game *game, float delta_time);
void OptionsScreenDraw(Game *game, uint8_t flags);

void MainStart(Game *game);

Vector2 WindowToTarget(Vector2 window_pos);

Vector2 GetVirtualMousePosition(Game *game);

#endif
