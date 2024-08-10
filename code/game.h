#pragma once

#include <stdint.h>

#include "raylib.h"
#include "raymath.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef float f32;
typedef uint8_t u8;

#define TILE_SIZE 64.0f

struct Player {
    Vector2 position;
    bool canJump;
    f32 vx;
    f32 vy;
};

struct Spikes {
    Vector2 position;
};

struct Goal {
    Vector2 position;
};

struct Level
{
    u32 width;
    u32 height;
    u32 tiles[2048];

    u32 spike_count;
    Spikes spikes[50];

    u32 goal_count;
    Goal goals[50];

    Vector2 spawn;
};

struct Game
{
    Player player[2];
    Level level[2];

    Camera2D camera[2];
    RenderTexture framebuffer[2];
    bool framebufferValid;

    bool horizontal_split;
};

enum TileId
{
    Tile_Air,
    Tile_Wall
};
