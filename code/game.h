#pragma once

#include <stdint.h>

#include "raymath.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef float f32;

struct Player {
    Vector2 position;
    f32 speed;
    bool canJump;
};

struct Level
{
    u32 width;
    u32 height;
    u32 *tiles;
};

