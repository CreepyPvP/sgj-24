#include <stdio.h>
#include <assert.h>

#include "loader.h"

#include "game.h"
#include "stb_image.h"

void LoadLevelFromFile(Level *level, char *path)
{
    level->spawn = { 400, 280 };

    i32 width;
    i32 height;
    i32 channel = 3;

    u8 *tmp = stbi_load(path, &width, &height, &channel, STBI_rgb);
    assert(tmp);

    level->width = width;
    level->height = height;

    level->fixed_camera = false;

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
               curr[1] == 255 &&
               curr[2] == 255){
                level->synchronizers[level->synchronizer_count].position.x = x * TILE_SIZE;
                level->synchronizers[level->synchronizer_count].position.y = y * TILE_SIZE;
                level->synchronizer_count++;
            } 
            else if(curr[0] == 0 &&
               curr[1] == 0 &&
               curr[2] == 255){
                level->spawn.x = x * TILE_SIZE + (TILE_SIZE/2);
                level->spawn.y = y * TILE_SIZE - (TILE_SIZE * 0.75f);
            } 
            else if(curr[0] == 255 &&
               curr[1] == 0 &&
               curr[2] == 255){
                level->fixed_camera = true;
                level->fixed_camera_pos.x = x * TILE_SIZE;
                level->fixed_camera_pos.y = y * TILE_SIZE;
            } 
            curr += 3;
        }
    }

}

void LoadGameFromFile(Game *game, u32 stage)
{
    game->horizontal_split = true;
    for (u32 i = 0; i < 2; ++i)
    {
        char path[1024];
        sprintf(path, "assets/levels/%upart%u.png", stage, i + 1);

        game->level[i] = {};
        LoadLevelFromFile(&game->level[i], path);

        game->player[i] = {};
        game->player[i].position = game->level[i].spawn;

        game->framebufferValid = false;
    }
}

