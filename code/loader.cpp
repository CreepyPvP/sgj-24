#include <stdio.h>
#include <assert.h>

#include "loader.h"

#include "game.h"
#include "stb_image.h"

void LoadLevelFromFile(Level *level, char *path)
{
    level->player.position = { 400, 280 };

    i32 width;
    i32 height;
    i32 channel = 3;

    u8 *tmp = stbi_load(path, &width, &height, &channel, STBI_rgb);
    assert(tmp);

    level->width = width;
    level->height = height;

    u8 *curr = tmp;
    for (u32 y = 0; y < level->height; ++y) {
        for (u32 x = 0; x < level->width; ++x) {

            if(curr[0] == 0 &&
               curr[1] == 0 &&
               curr[2] == 0){
                level->tiles[x + y * level->width] = Tile_Wall;
            }
            else {
                level->tiles[x + y * level->width] = Tile_Air;
            }
            
            
            if(curr[0] == 255 &&
               curr[1] == 0 &&
               curr[2] == 0){
                level->spikes[level->spike_count].position.x = x * TILE_SIZE;
                level->spikes[level->spike_count].position.y = y * TILE_SIZE;
                level->spike_count++;
            } 
            else if(curr[0] == 0 &&
               curr[1] == 255 &&
               curr[2] == 0){
                level->goals[level->goal_count].position.x = x * TILE_SIZE;
                level->goals[level->goal_count].position.y = y * TILE_SIZE;
                level->goal_count++;
            } 
            else if(curr[0] == 0 &&
               curr[1] == 0 &&
               curr[2] == 255){
                level->player.position.x = x * TILE_SIZE + (TILE_SIZE/2);
                level->player.position.y = y * TILE_SIZE - (TILE_SIZE * 0.75f);
            } 
            curr += 3;
        }
    }

}

void LoadGameFromFile(Game *game, u32 stage){
    char path1[1024];
    char path2[1024];
    sprintf(path1, "assets/levels/%upart1.png", stage);
    sprintf(path2, "assets/levels/%upart2.png", stage);

    *game = {};
    LoadLevelFromFile(&game->level[0], path1);
    LoadLevelFromFile(&game->level[1], path2);
}

