#include <stdio.h>
#include <assert.h>

#include "loader.h"

#include "game.h"
#include "stb_image.h"

Level LoadFromFile(u32 stage, u32 *buffer)
{
    Level level = {};
    level.tiles = buffer;

    // char path[1024];
    // sprintf(path, "assets/levels/1part1.png");
    // sprintf(path, "C:/Users/eario/Desktop/sgj24/sgj-24/assets/levels/1part1.png");

    i32 width;
    i32 height;
    i32 channel = 3;

    u8* tmp = stbi_load("assets/levels/test_level.png", &width, &height, &channel, 0);
    assert(tmp);

    u8* curr = tmp;
    for (u32 y = 0; y < level.height; ++y) {
        for (u32 x = 0; x < level.width; ++x) {
            if(curr[0] == 0 &&
               curr[1] == 0 &&
               curr[2] == 0){
                level.tiles[x + y * level.width] = 1;
            }
            curr += 3;
        }
    }

    return level;
}
