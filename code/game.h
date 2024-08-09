#pragma once

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

