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

Vector2 ScaledVec2(Vector2 vec2) {
	return (Vector2) {
		.x = vec2.x * global_game_scale.x,
		.y = vec2.y * global_game_scale.y
	};
}

Rectangle ScaledRec(Rectangle rec) {
	return (Rectangle) {
		.x = rec.x * global_game_scale.x,
		.y = rec.y * global_game_scale.y,
		.width = rec.width * global_game_scale.x,
		.height = rec.height * global_game_scale.y,
	};
}

Vector2 ScaledVec2WithCamera(Vector2 vec2, Camera2D *camera) {
	vec2 = ScaledVec2(vec2);
	vec2 = GetScreenToWorld2D(vec2, *camera);

	return vec2;
}

Rectangle ScaledRecWithCamera(Rectangle rec, Camera2D *camera) {
	Vector2 pos = ScaledVec2WithCamera((Vector2) { rec.x, rec.y }, camera);
	Vector2 size = ScaledVec2((Vector2) { rec.width, rec.height });

	rec = (Rectangle) {
		.x = pos.x, 
		.y = pos.y,
		.width = size.x,
		.height = size.y
	};

	return rec;
};

