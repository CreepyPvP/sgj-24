#pragma once

#include "game.h"

inline f32 Abs(f32 a)
{
    if (a > 0) 
    {
        return a;
    }

    return -a;
}

inline f32 Min(f32 a, f32 b)
{
    if (a < b)
    {
        return a;
    }

    return b;
}

inline f32 Min(f32 a, f32 b, f32 c)
{
    return Min(a, Min(b, c));
}

inline f32 Max(f32 a, f32 b)
{
    if (a > b)
    {
        return a;
    }

    return b;
}

inline f32 Max(f32 a, f32 b, f32 c)
{
    return Max(a, Max(b, c));
}




inline bool aabb_contains_point(Vector2 aabb_pos, Vector2 aabb_size, Vector2 point)
{
    return aabb_pos.x <= point.x && point.x <= aabb_pos.x + aabb_size.x &&
           aabb_pos.y <= point.y && point.y <= aabb_pos.y + aabb_size.y;
}

inline bool aabb_intersects_aabb(Vector2 aabb1_pos, Vector2 aabb1_size, Vector2 aabb2_pos, Vector2 aabb2_size)
{
    return aabb_contains_point(aabb1_pos, aabb1_size, aabb2_pos) ||
           aabb_contains_point(aabb1_pos, aabb1_size, {aabb2_pos.x + aabb2_size.x , aabb2_pos.y + aabb2_size.y }) ;
}