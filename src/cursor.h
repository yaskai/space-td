#include <stdint.h>
#include "raylib.h"

#ifndef CURSOR_H_
#define CURSOR_H_

#define CURSOR_OPEN_SELECTION	0x01

typedef struct {
	Rectangle selection_rec;

	Vector2 screen_position;
	Vector2 world_position;
	Vector2 click_position;

	uint8_t flags;

} Cursor;

void CursorUpdate(Cursor *cursor, float dt);
void CursorDraw(Cursor *cursor);

#endif
