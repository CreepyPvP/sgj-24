#include <stdio.h>
#include <assert.h>

#include "loader.h"

#include "game.h"
#include "stb_image.h"

Level LoadFromFile(u32 stage, u32 *buffer)
{
    Level level = {};
    level.tiles = buffer;

    char path[1024];
    sprintf(path, "assets/levels/test_level.png");

    i32 width;
    i32 height;
    i32 channel = 3;

    u8* tmp = stbi_load(path, &width, &height, &channel, STBI_rgb);
    assert(tmp);

    level.width = width;
    level.height = height;

    printf("%d\n", level.width);
    printf("%d\n", level.height);

    u8* curr = tmp;
    for (u32 y = 0; y < level.height; ++y) {
        for (u32 x = 0; x < level.width; ++x) {

            if(curr[0] == 255 &&
               curr[1] == 255 &&
               curr[2] == 255){
                level.tiles[x + y * level.width] = Tile_Air;
            }
            else if(curr[0] == 0 &&
               curr[1] == 0 &&
               curr[2] == 0){
                level.tiles[x + y * level.width] = Tile_Wall;
            }
            else if(curr[0] == 255 &&
               curr[1] == 0 &&
               curr[2] == 0){
                level.tiles[x + y * level.width] = Tile_Spikes;
            } 
            else if(curr[0] == 0 &&
               curr[1] == 255 &&
               curr[2] == 0){
                level.tiles[x + y * level.width] = Tile_Goal;
            } 
            else if(curr[0] == 0 &&
               curr[1] == 0 &&
               curr[2] == 255){
                level.tiles[x + y * level.width] = Tile_Player;
            } 
            else {
                level.tiles[x + y * level.width] = Tile_Air;
            }
            curr += 3;
        }
    }

    return level;
}
