#include "game_math.h"

u32 halton2Index = 0;

f32 halton2[256];


f32 Halton(u32 i, u32 b)
{
    f32 f = 1;
    f32 r = 0;
    while (i > 0) {
        f = f / b;
        r += f * (i % b);
        i = floor(i / b);
    }

    return r;
}

f32 NextHalton2()
{
    halton2Index = (halton2Index + 1) & 255;
    return halton2[halton2Index];
}

void InitializeSamplers()
{
    for (u32 i = 0; i < 255; ++i)
    {
        halton2[i] = Halton(i, 2);
    }
}

