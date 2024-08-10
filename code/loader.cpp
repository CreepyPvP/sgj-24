#include <stdio.h>
#include <assert.h>

#include "loader.h"

#include "game.h"
#include "stb_image.h"



void LoadGameFromFile(Game* game, u32 stage_number){
    LoadLevelFromFile(&game->level[0],stage_number, true);
    LoadLevelFromFile(&game->level[1],stage_number, false);
    game->horizontal_split = false;    
}

void LoadLevelFromFile(Level* level, u32 stage, bool part_one)
{
    level->player.position = { 400, 280 };
    level->player.speed = 0;
    level->player.canJump = false;


    char path[1024];
    if(part_one){
        sprintf(path, "assets/levels/%upart1.png", stage);
    }else{
        sprintf(path, "assets/levels/%upart2.png", stage);
    }

    i32 width;
    i32 height;
    i32 channel = 3;

    u8* tmp = stbi_load(path, &width, &height, &channel, STBI_rgb);
    assert(tmp);

    level->width = width;
    level->height = height;

    level->spike_count = 0;
    level->goal_count = 0;

    u8* curr = tmp;
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


