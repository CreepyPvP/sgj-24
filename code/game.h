#pragma once

#include <stdint.h>

#include "raymath.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef float f32;
typedef uint8_t u8;

#define TILE_SIZE 64.0f

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

struct Game
{
    Level level[2];
    Player player[2];
    bool horizontal_split;
};

enum TileId
{
    Tile_Air,
    Tile_Wall,
    Tile_Goal,
    Tile_Spikes,
};
