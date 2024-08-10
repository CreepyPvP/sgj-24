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

inline f32 Max(f32 a, f32 b)
{
    if (a > b)
    {
        return a;
    }

    return b;
}
