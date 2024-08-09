#include <stdio.h>

#include "game.h"
#include "stb_image.h"
#include <cassert>


Level load_from_file(u32 stage){

    u32 buffer[1024];
    Level level = {};
    level.tiles = buffer;

    Level* levelpointer = &level;

    char path1[1024];
    sprintf(path1, "C:/Users/eario/Desktop/sgj24/sgj-24/assets/levels/1part1.png");
    //sprintf(path1, "../assets/levels/%upart1.png", stage);
    u8* tmp = stbi_load(path1, (i32*) &levelpointer->width, (i32*) &levelpointer->height, NULL, STBI_rgb);
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