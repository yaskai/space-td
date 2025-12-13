#include "raylib.h"

#ifndef KMATH_H_
#define KMATH_H_

void ScaleInit(Rectangle source, Rectangle dest);

Vector2 ScaledVec2(Vector2 vec2);
Rectangle ScaledRec(Rectangle rec);

Vector2 ScaledVec2WithCamera(Vector2 vec2, Camera2D *camera);
Rectangle ScaledRecWithCamera(Rectangle rec, Camera2D *camera);

#endif
