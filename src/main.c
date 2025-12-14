#include <stdio.h>
#include "raylib.h"
#include "config.h"
#include "game.h"
 
int main() {
	// Initialize audio backend
	InitAudioDevice();
	SetMasterVolume(1);
	
	// Disable logs
	SetTraceLogLevel(LOG_ERROR);

	// Initialize game
	// Set window options, instantiate objects, allocate memory, etc.
	Game game = {0};
	GameInit(&game);

	// Open window, use values from config file
	//SetConfigFlags(FLAG_BORDERLESS_WINDOWED_MODE | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(game.conf.window_width, game.conf.window_height, "Game");
	SetTargetFPS(game.conf.refresh_rate);
	//HideCursor();
	
	// Load empty texture for use as a buffer
	// Buffer is drawn and scaled to window resolution
	GameRenderInit(&game);

	// Load game assets: spritesheets, audio, etc.
	GameContentInit(&game);

	//SetExitKey(KEY_F10);	
	bool exit = false;

	// Main loop:
	while(!exit) {
		exit = ( WindowShouldClose() ^ (game.flags & GAME_QUIT_REQUEST) );	

		// Update game logic
		GameUpdate(&game);

		// Render to buffer
		GameDrawToBuffer(&game, 0);

		// Render to screen
		GameDrawToWindow(&game);
	}

	// Cleanup:
	// Free allocated memory, close application
	GameClose(&game);
	CloseAudioDevice();
	CloseWindow();

	return 0;
}

