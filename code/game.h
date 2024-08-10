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

    u32 animation_frame;
    u32 state;
};

enum PlayerStateId{
    IdleRight = 0,
    IdleLeft = 1,
    RunRight = 2,
    RunLeft = 3
};

struct Spikes {
    Vector2 position;
    u32 rotation;
};

struct Goal {
    Vector2 position;
};

struct Synchronizer {
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

    u32 synchronizer_count;
    Synchronizer synchronizers[50];

    Vector2 spawn;

    bool fixed_camera;
    Vector2 fixed_camera_pos;
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
    Tile_WallUp,
    Tile_WallDown,
    Tile_WallLeft,
    Tile_WallRight,

    Tile_CornerUpLeft,
    Tile_CornerUpRight,
    Tile_CornerDownLeft,
    Tile_CornerDownRight,

    Tile_PlatformLeft,
    Tile_PlatformRight,

    Tile_WallFull,

    Tile_Walls,                 // Used to compare type wall

    Tile_Air,
};
