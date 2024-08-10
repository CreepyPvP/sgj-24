#pragma once

#include <stdint.h>

#include "raymath.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef float f32;
typedef uint8_t u8;

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
    Tile_Air = 0,
    Tile_Wall = 1,
    Tile_Player = 2,
    Tile_Goal = 3,
    Tile_Spikes = 4
};

