#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "loader.h"

#include "game_math.h"
#include "game.h"
#include "stb_image.h"


u8 tileRuleLookup[256];

inline u8 NeighbourMask(u8 a, u8 b, u8 c, 
                        u8 d,       u8 e, 
                        u8 f, u8 g, u8 h)
{
    return a << 0 | b << 1 | c << 2 | 
           d << 3 | e << 4 | f << 5 | 
           g << 6 | h << 7;
}

void PopulateTileRuleLookup()
{
    memset(tileRuleLookup, Tile_WallFull, sizeof(tileRuleLookup));

    // Fuck my life
    // Edges

    tileRuleLookup[NeighbourMask(0, 0, 0,
                                 1,    1,
                                 1, 1, 1)] = Tile_WallUp;

    tileRuleLookup[NeighbourMask(1, 0, 0,
                                 1,    1,
                                 1, 1, 1)] = Tile_WallUp;
    
    tileRuleLookup[NeighbourMask(0, 0, 1,
                                 1,    1,
                                 1, 1, 1)] = Tile_WallUp;

    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 1,    1,
                                 1, 0, 0)] = Tile_WallDown;
    
    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 1,    1,
                                 0, 0, 1)] = Tile_WallDown;

    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 1,    1,
                                 0, 0, 0)] = Tile_WallDown;

    tileRuleLookup[NeighbourMask(0, 1, 1,
                                 0,    1,
                                 0, 1, 1)] = Tile_WallLeft;

    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 0,    1,
                                 0, 1, 1)] = Tile_WallLeft;

    tileRuleLookup[NeighbourMask(0, 1, 1,
                                 0,    1,
                                 1, 1, 1)] = Tile_WallLeft;

    tileRuleLookup[NeighbourMask(1, 1, 0,
                                 1,    0,
                                 1, 1, 0)] = Tile_WallRight;

    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 1,    0,
                                 1, 1, 0)] = Tile_WallRight;
    
    tileRuleLookup[NeighbourMask(1, 1, 0,
                                 1,    0,
                                 1, 1, 1)] = Tile_WallRight;

    // Corners

    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 1,    1,
                                 1, 1, 0)] = Tile_CornerUpLeft;

    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 1,    1,
                                 0, 1, 1)] = Tile_CornerUpRight;

    tileRuleLookup[NeighbourMask(1, 1, 0,
                                 1,    1,
                                 1, 1, 1)] = Tile_CornerDownLeft;

    tileRuleLookup[NeighbourMask(0, 1, 1,
                                 1,    1,
                                 1, 1, 1)] = Tile_CornerDownRight;

    // Double corners
    

    tileRuleLookup[NeighbourMask(0, 1, 0,
                                 1,    1,
                                 1, 1, 1)] = Tile_DoubleCornerUp;

    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 1,    1,
                                 0, 1, 0)] = Tile_DoubleCornerDown;
    
    tileRuleLookup[NeighbourMask(1, 1, 0,
                                 1,    1,
                                 1, 1, 0)] = Tile_DoubleCornerRight;

    tileRuleLookup[NeighbourMask(0, 1, 1,
                                 1,    1,
                                 0, 1, 1)] = Tile_DoubleCornerLeft;

    // Outer edges
    

    tileRuleLookup[NeighbourMask(0, 0, 0,
                                 0,    1,
                                 0, 1, 1)] = Tile_OuterUpLeft;
    
    tileRuleLookup[NeighbourMask(0, 0, 0,
                                 1,    0,
                                 1, 1, 0)] = Tile_OuterUpRight;
    
    tileRuleLookup[NeighbourMask(0, 1, 1,
                                 0,    1,
                                 0, 0, 0)] = Tile_OuterDownLeft;

    tileRuleLookup[NeighbourMask(1, 1, 0,
                                 1,    0,
                                 0, 0, 0)] = Tile_OuterDownRight;

    // Pipes
    

    tileRuleLookup[NeighbourMask(0, 0, 0,
                                 1,    1,
                                 0, 0, 0)] = Tile_PipeHorizontal;

    tileRuleLookup[NeighbourMask(1, 0, 0,
                                 1,    1,
                                 1, 0, 0)] = Tile_PipeHorizontal;

    tileRuleLookup[NeighbourMask(0, 0, 1,
                                 1,    1,
                                 0, 0, 1)] = Tile_PipeHorizontal;

    tileRuleLookup[NeighbourMask(0, 1, 0,
                                 0,    0,
                                 0, 1, 0)] = Tile_PipeVertical;
    
    tileRuleLookup[NeighbourMask(1, 1, 1,
                                 0,    0,
                                 0, 1, 0)] = Tile_PipeVertical;

    tileRuleLookup[NeighbourMask(0, 1, 0,
                                 0,    0,
                                 1, 1, 1)] = Tile_PipeVertical;

    // Platforms


    tileRuleLookup[NeighbourMask(0, 0, 0,
                                 0,    1,
                                 0, 0, 0)] = Tile_PlatformLeft;
    
    tileRuleLookup[NeighbourMask(0, 0, 0,
                                 1,    0,
                                 0, 0, 0)] = Tile_PlatformRight;

    tileRuleLookup[NeighbourMask(0, 0, 0,
                                 1,    1,
                                 0, 0, 0)] = Tile_WallUp;
}

inline bool IsWallOrOutside(i32 x, i32 y, i32 width, i32 height, u8 *buffer)
{
    if (x < 0 || y < 0 || x >= width || y >= height)
    {
        return true;
    }

    return buffer[x + y * width];
}

inline bool IsWall(i32 x, i32 y, i32 width, i32 height, u8 *buffer)
{
    return buffer[x + y * width];
}

void LoadLevelFromFile(Level *level, char *path)
{
    level->spawn = { 400, 280 };

    i32 width;
    i32 height;
    i32 channel = 3;

    u8 *tmp = stbi_load(path, &width, &height, &channel, STBI_rgb);
    if (!tmp)
    {
        TraceLog(LOG_FATAL, "Failed to open file %s", path);
        assert(0);
    }

    level->width = width;
    level->height = height;

    u8 wallMask[2048];
    level->fixed_camera = false;

    u8 *curr = tmp;
    for (u32 y = 0; y < level->height; ++y) {
        for (u32 x = 0; x < level->width; ++x) {

            wallMask[x + y * level->width] = (curr[0] == 0 && curr[1] == 0 && curr[2] == 0);
            
            if(curr[0] == 255 && curr[1] == 0 && curr[2] <= 3) {
                level->spikes[level->spike_count].position.x = x * TILE_SIZE;
                level->spikes[level->spike_count].position.y = y * TILE_SIZE;
                level->spikes[level->spike_count].rotation = curr[2];
                level->spike_count++;
            } 
            else if(curr[0] == 0 && curr[1] == 255 && curr[2] == 0) {
                level->goals[level->goal_count].position.x = x * TILE_SIZE;
                level->goals[level->goal_count].position.y = y * TILE_SIZE;
                level->goal_count++;
            } 
            else if(curr[0] == 0 && curr[1] == 255 && curr[2] == 255) {
                level->synchronizers[level->synchronizer_count].position.x = x * TILE_SIZE;
                level->synchronizers[level->synchronizer_count].position.y = y * TILE_SIZE;
                level->synchronizers[level->synchronizer_count].playing_music = false;
                level->synchronizer_count++;
            } 
            else if(curr[0] == 0 && curr[1] == 0 && curr[2] == 255) {
                level->spawn.x = x * TILE_SIZE + (TILE_SIZE/2);
                level->spawn.y = y * TILE_SIZE - (TILE_SIZE * 0.75f);
            } 
            else if(curr[0] == 255 && curr[1] <= 1 && curr[2] == 255) {
                level->fixed_camera = true;
                level->fixed_camera_pos.x = x * TILE_SIZE;
                level->fixed_camera_pos.y = y * TILE_SIZE;
                if(curr[1] == 1){
                    wallMask[x + y * level->width] = 1;
                }
            } 
            else if (curr[0] == 100 && curr[1] == 100 && curr[2] == 100)
            {
                wallMask[x + y * level->width] = 1;
            }

            curr += 3;
        }
    }

    for (i32 x = 0; x < level->width; ++x)
    {
        for (i32 y = 0; y < level->height; ++y)
        {
            if (!IsWall(x, y, level->width, level->height, wallMask)) 
            {
                level->tiles[x + y * width] = Tile_Air;
                continue;
            }

            // 0 1 2
            // 3   4
            // 5 6 7
            u8 lookup = 0;
            lookup |= IsWallOrOutside(x - 1, y - 1, level->width, level->height, wallMask) << 0;
            lookup |= IsWallOrOutside(x    , y - 1, level->width, level->height, wallMask) << 1;
            lookup |= IsWallOrOutside(x + 1, y - 1, level->width, level->height, wallMask) << 2;
            lookup |= IsWallOrOutside(x - 1, y    , level->width, level->height, wallMask) << 3;
            lookup |= IsWallOrOutside(x + 1, y    , level->width, level->height, wallMask) << 4;
            lookup |= IsWallOrOutside(x - 1, y + 1, level->width, level->height, wallMask) << 5;
            lookup |= IsWallOrOutside(x    , y + 1, level->width, level->height, wallMask) << 6;
            lookup |= IsWallOrOutside(x + 1, y + 1, level->width, level->height, wallMask) << 7;

            level->tiles[x + y * width] = tileRuleLookup[lookup];
        }
    }
}

void LoadGameFromFile(Game *game, u32 stage)
{
    bool horizontal_split = true;
    if (game->horizontal_split != horizontal_split)
    {
        game->framebufferValid = false;
    }
    game->horizontal_split = horizontal_split;

    for (u32 i = 0; i < 2; ++i)
    {
        char path[1024];
        sprintf(path, "assets/levels/%upart%u.png", stage, i + 1);

        game->level[i] = {};
        LoadLevelFromFile(&game->level[i], path);

        game->player[i] = {};
        game->player[i].position = game->level[i].spawn;
    }

    for (u32 i = 0; i < STAR_COUNT; i++)
    {
        Star star = {};
        star.x = Halton(i + 100 * stage, 2);
        star.y = Halton(i + 100 * stage, 3);
        star.size = Halton(i + 100 * stage, 5);
        star.frequency = Halton(i + 100 * stage, 7);
        game->star[i] = star;
    }
}

