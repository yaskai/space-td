#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include "config.h"
#include "sprites.h"

Texture2D controls;

int fps = 60;

// Buffer texture game draws to, used for scaling graphics to desired resolution 
RenderTexture2D render_target;

// Game state update and draw function type defines
typedef void(*UpdateFunc)(Game *game, float dt);
typedef void(*DrawFunc)(Game *game, uint8_t flags);

// Game state update and draw function arrays, state acts as index 
// (ie. state = main, game_update_fn[main] called in GameUpdate)
UpdateFunc game_update_fn[] = { TitleUpdate, MainUpdate, OverScreenUpdate };
DrawFunc game_draw_fn[] = { TitleDraw, MainDraw, OverScreenDraw };

// Initialize data, allocate memory, etc.
void GameInit(Game *game) {
	// Initialize config struct and read options from file
	game->conf = (Config){0};
	ConfigRead(&game->conf, "options.conf");

	fps = game->conf.refresh_rate;

	// Initialize camera
	game->cam = (Camera2D) {
		.target = {0, 0},
		.offset = Vector2Scale((Vector2){VIRTUAL_WIDTH, VIRTUAL_HEIGHT}, 0.5f),
		.rotation = 0.0f,
		.zoom = 1.0f
	};

	game->cursor = (Cursor){0};

	HandlerInit(&game->handler, 0);

	MainStart(game);
}

// Initialize necessary data for rendering the game 
void GameRenderInit(Game *game) {
	// Load empty texture, used as buffer for scaling
	render_target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
	SetTextureFilter(render_target.texture, TEXTURE_FILTER_POINT);

	// Set source and destination rectangle values for window scaling
	game->render_src_rec  = (Rectangle) { 0, 0, VIRTUAL_WIDTH, -VIRTUAL_HEIGHT };
	game->render_dest_rec = (Rectangle) { 0, 0, game->conf.window_width, game->conf.window_height };
}

// Initialize sprite loader struct, load assets
void GameContentInit(Game *game) {
}

void GameUpdate(Game *game) {
	// Get delta time once only, pass to other update functions
	float delta_time = GetFrameTime();

	// Send quit request on hitting escape
	if(IsKeyPressed(KEY_ESCAPE))
		game->flags |= GAME_QUIT_REQUEST;

	// Call state appropriate update function
	game_update_fn[game->state](game, delta_time);
}

// Render game to buffer texture
void GameDrawToBuffer(Game *game, uint8_t flags) {
	BeginTextureMode(render_target);
	ClearBackground((Color){0});

	// Call state appropriate draw function
	game_draw_fn[game->state](game, flags);

	EndTextureMode();
}

// Render buffer onto window
void GameDrawToWindow(Game *game) {
	BeginDrawing();

	ClearBackground((Color){0});
	
	// Draw scaled render_target texture to window 
	DrawTexturePro(render_target.texture, game->render_src_rec, game->render_dest_rec, Vector2Zero(), 0, WHITE);

	// Draw overlays
	DrawFPS(0, 0);
	CursorDraw(&game->cursor);

	EndDrawing();
}

// Free allocated memory for buffer texture and assets 
void GameClose(Game *game) {
	UnloadRenderTexture(render_target);
}

// Update title screen UI elements, start gameplay on user input
void TitleUpdate(Game *game, float delta_time) {

	if(IsKeyPressed(KEY_SPACE))
		MainStart(game);
}

// Draw title screen graphics
void TitleDraw(Game *game, uint8_t flags) {
}

// Main gameplay loop logic
void MainUpdate(Game *game, float delta_time) {
	CursorUpdate(&game->cursor, delta_time);

	HandlerUpdate(&game->handler, delta_time);
}

// Render objects to buffer texture
void MainDraw(Game *game, uint8_t flags) {
	HandlerDraw(&game->handler);
}

void OverScreenUpdate(Game *game, float delta_time) {
}

void OverScreenDraw(Game *game, uint8_t flags) {
}

void OptionsScreenUpdate(Game *game, float delta_time) {
}

void OptionsScreenDraw(Game *game, uint8_t flags) {
}

// Start gameplay
void MainStart(Game *game) {
	game->state = GAME_MAIN;
}

