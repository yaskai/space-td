#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "kmath.h"

Vector2 global_game_scale;

void ScaleInit(Rectangle source, Rectangle dest) {
	global_game_scale.x = fabs(source.width / dest.width);
	global_game_scale.y = fabs(source.height / dest.height);
	printf("game scale: %f, %f\n", global_game_scale.x, global_game_scale.y);
}

Rectangle ScaledRec(Rectangle rec) {
	return (Rectangle) {
		.x = rec.x * global_game_scale.x,
		.y = rec.y * global_game_scale.y,
		.width = rec.width * global_game_scale.x,
		.height = rec.height * global_game_scale.y,
	};
}

