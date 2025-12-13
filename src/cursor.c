#include "raylib.h"
#include "raymath.h"
#include "cursor.h"
#include "handler.h"
#include "game.h"
#include "kmath.h"

void CursorUpdate(Cursor *cursor, Handler *handler, Camera2D *camera, float dt) {
	// Set world and screen positions
	cursor->screen_position = GetMousePosition();
	cursor->world_position = GetScreenToWorld2D(cursor->screen_position, *camera);

	// On press
	if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		// Start selection box setting
		if(!(cursor->flags & CURSOR_OPEN_SELECTION)) { 
			cursor->flags |= CURSOR_OPEN_SELECTION;
			cursor->click_position = cursor->screen_position;
			return;
		}

		// Continue selection box setting 
		Vector2 box_min = Vector2Min(cursor->click_position, cursor->screen_position);	
		Vector2 box_max = Vector2Max(cursor->click_position, cursor->screen_position);	
		Vector2 box_dim = Vector2Subtract(box_max, box_min);
		
		// Set rectangle values
		cursor->selection_rec = (Rectangle) {
			.x = box_min.x,
			.y = box_min.y,
			.width = box_dim.x,
			.height = box_dim.y
		};
	}

	// On release
	if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		// Clear selection box
		if(cursor->flags & CURSOR_OPEN_SELECTION) {

			CheckSelectedUnits(handler, cursor->selection_rec);

			cursor->selection_rec = (Rectangle) { 0 };
			cursor->flags &= ~CURSOR_OPEN_SELECTION;
		}	
	}
}

void CursorDraw(Cursor *cursor) {
	DrawCircleV(cursor->screen_position, 5, SKYBLUE);

	if(cursor->flags & CURSOR_OPEN_SELECTION) {
		DrawRectangleRec(cursor->selection_rec, ColorAlpha(RAYWHITE, 0.5f));
		DrawRectangleLinesEx(cursor->selection_rec, 2, RAYWHITE);
	}
}

void CursorCameraControls(Cursor *cursor, Camera2D *camera, float dt) {
	Vector2 key_direction = Vector2Zero();	
	
	if(IsKeyDown(KEY_A)) key_direction.x--;
	if(IsKeyDown(KEY_D)) key_direction.x++;

	if(IsKeyDown(KEY_W)) key_direction.y--;
	if(IsKeyDown(KEY_S)) key_direction.y++;

	camera->target = Vector2Add(camera->target, Vector2Scale(key_direction, 1000 * dt));
}

