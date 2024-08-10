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
