#pragma once

#include <stdint.h>

#include "raylib.h"
#include "raymath.h"

typedef int32_t i32;
typedef uint32_t u32;
typedef float f32;
typedef uint8_t u8;

#define TILE_SIZE 64.0f

#define STAR_COUNT 64
#define RAIN_PARTICLE_COUNT 64

struct Player 
{
    Vector2 position;
    bool canJump;
    f32 vx;
    f32 vy;

    u32 animation_frame;
    u32 state;
};

enum PlayerStateId
{
    IdleRight,
    IdleLeft,
    RunRight,
    RunLeft
};

struct Spikes 
{
    Vector2 position;
    u32 rotation;
};

struct Goal 
{
    Vector2 position;
};

struct Synchronizer 
{
    Vector2 position;
    bool playing_music;
};

struct Level
{
    i32 width;
    i32 height;
    u32 tiles[2048];

    u32 spike_count;
    Spikes spikes[64];

    u32 goal_count;
    Goal goals[64];

    u32 synchronizer_count;
    Synchronizer synchronizers[32];

    Vector2 spawn;

    bool fixed_camera;
    Vector2 fixed_camera_pos;
};

struct Star
{
    // In range 0..1
    f32 x;
    f32 y;
    f32 size;
    f32 frequency;
};

struct Game
{
    Camera2D camera[2];
    Player player[2];
    Level level[2];

    f32 framebufferSize[2];
    bool framebufferValid;
    RenderTexture framebuffer[2];

    bool horizontal_split;

    // Fancy stuff...

    // For blue level only
    Star star[STAR_COUNT];

    // For red level only
    Vector2 rainPosition[2][RAIN_PARTICLE_COUNT];
    f32 timeUntilNextFlash;
    f32 flashIntensity;
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

    Tile_DoubleCornerUp,
    Tile_DoubleCornerDown,
    Tile_DoubleCornerLeft,
    Tile_DoubleCornerRight,

    Tile_OuterUpLeft,
    Tile_OuterUpRight,
    Tile_OuterDownLeft,
    Tile_OuterDownRight,

    Tile_PipeHorizontal,
    Tile_PipeVertical,

    Tile_PlatformLeft,
    Tile_PlatformRight,

    Tile_WallFull,

    Tile_Walls,                 // Used to compare type wall

    Tile_Air,
};
