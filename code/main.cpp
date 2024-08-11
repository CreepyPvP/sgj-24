#include "raylib.h"
#include "game.h"
#include "loader.h"
#include "game_math.h"

#include <cassert>
#include <stdio.h>

#define G 1500.0f
#define PLAYER_JUMP_SPD -800.0f
#define PLAYER_HOR_SPD 450.0f
#define PLAYER_HEIGHT 1.65f

#define TOTAL_LEVEL_COUNT 12

u32 level_order[TOTAL_LEVEL_COUNT] = {0,1,6,7,3,4,12,5,11,2,8,9};

u32 current_level;

bool reset_level;

Color clearColors[2] = { 
    {64, 0, 128, 255}, 
    {210, 152, 181, 255}
};

Color rainColors[2] = {
    { 200, 200, 200, 150 },
    { 255, 255, 255, 150 },
};

f32 rainVelocity[2] = {
    127,
    250,
};

struct DebugRay
{
    Vector2 start;
    Vector2 end;
    Color color;
};

u32 ray_count;
DebugRay rays[100];

DebugRay *AllocRay()
{
    return &rays[ray_count++];
}

Rectangle spike[2][4];
Rectangle tile[2][Tile_Walls];
Rectangle puddle[2];
Texture2D tileset;

Texture2D player_sprite_texture;
Rectangle player_frame_rec;

inline Rectangle TileAt(u32 x, u32 y)
{
    return { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
}

void LoadAssets(){
    tileset = LoadTexture("assets/tileset.png");

    puddle[0] = { 0, 400, 16, 16 };
    puddle[1] = { 0, 416, 16, 16 };

    spike[0][0] = TileAt(3, 6);
    spike[1][0] = TileAt(3, 8);
    spike[0][1] = TileAt(3, 3);
    spike[1][1] = TileAt(1, 3);
    spike[0][2] = TileAt(6, 3);
    spike[1][2] = TileAt(6, 1);
    spike[0][3] = TileAt(6, 6);
    spike[1][3] = TileAt(8, 6);
    
    // Blue: 0, Pink: 1
    // Fuck...
    tile[0][Tile_WallUp] = TileAt(1, 6);
    tile[1][Tile_WallUp] = TileAt(1, 8);
    tile[0][Tile_WallDown] = TileAt(8, 3);
    tile[1][Tile_WallDown] = TileAt(8, 1);
    tile[0][Tile_WallLeft] = TileAt(6, 8);
    tile[1][Tile_WallLeft] = TileAt(8, 8);
    tile[0][Tile_WallRight] = TileAt(3, 1);
    tile[1][Tile_WallRight] = TileAt(1, 1);

    tile[0][Tile_CornerUpLeft] = TileAt(5, 2);
    tile[1][Tile_CornerUpLeft] = TileAt(5, 0);
    tile[0][Tile_CornerUpRight] = TileAt(7, 5);
    tile[1][Tile_CornerUpRight] = TileAt(9, 5);
    tile[0][Tile_CornerDownLeft] = TileAt(2, 4);
    tile[1][Tile_CornerDownLeft] = TileAt(0, 4);
    tile[0][Tile_CornerDownRight] = TileAt(4, 7);
    tile[1][Tile_CornerDownRight] = TileAt(4, 9);

    tile[0][Tile_OuterUpLeft] = TileAt(5, 7);
    tile[1][Tile_OuterUpLeft] = TileAt(5, 9);
    tile[0][Tile_OuterUpRight] = TileAt(2, 5);
    tile[1][Tile_OuterUpRight] = TileAt(0, 5);
    tile[0][Tile_OuterDownLeft] = TileAt(7, 4);
    tile[1][Tile_OuterDownLeft] = TileAt(9, 4);
    tile[0][Tile_OuterDownRight] = TileAt(4, 2);
    tile[1][Tile_OuterDownRight] = TileAt(4, 0);

    tile[0][Tile_PipeHorizontal] = TileAt(7, 3);
    tile[1][Tile_PipeHorizontal] = TileAt(7, 1);
    tile[0][Tile_PipeVertical] = TileAt(3, 2);
    tile[1][Tile_PipeVertical] = TileAt(1, 2);

    tile[0][Tile_PlatformLeft] = TileAt(4, 6);
    tile[1][Tile_PlatformLeft] = TileAt(4, 8);
    tile[0][Tile_PlatformRight] = TileAt(5, 6);
    tile[1][Tile_PlatformRight] = TileAt(4, 8);

    tile[0][Tile_WallFull] = TileAt(2, 3);
    tile[1][Tile_WallFull] = TileAt(0, 3);

    Image player_sprite_image = LoadImage("assets/player/SpriteSheet.png");
    ImageResize(&player_sprite_image,384,448);
    player_sprite_texture = LoadTextureFromImage(player_sprite_image);
    UnloadImage(player_sprite_image);

    player_frame_rec = {0.0f, 0.0f, (float) player_sprite_texture.width / 6, (float) player_sprite_texture.height / 4 };
}

enum Direction
{
    Direction_Up,
    Direction_Down,
    Direction_Left,
    Direction_Right,
};

Vector2 DirToVec(Direction direction)
{
    switch (direction) {
        case Direction_Up: {
            return { 0, -1 };
        }
        case Direction_Down: {
            return { 0, 1 };
        }
        case Direction_Left: {
            return { -1, 0 };
        }
        case Direction_Right: {
            return { 1, 0 };
        }
    }

    return {};
}

f32 Raycast(Player *player, Level *level, Vector2 offset, Direction direction)
{
    f32 maxDist = 5 * TILE_SIZE;
    bool horizontal = direction == Direction_Left || direction == Direction_Right;
    Vector2 dir = DirToVec(direction);

    DebugRay *ray = AllocRay();
    ray->start = Vector2Add(player->position, offset);
    ray->color = BLACK;
    
    f32 t = 0;
    f32 gridOffset;

    if (direction == Direction_Down)
    {
        gridOffset = ((ray->start.y / TILE_SIZE) - (i32) (ray->start.y / TILE_SIZE)) * TILE_SIZE;
    }
    if (direction == Direction_Up)
    {
        gridOffset = TILE_SIZE - (((ray->start.y / TILE_SIZE) - (i32) (ray->start.y / TILE_SIZE)) * TILE_SIZE);
    }
    if (direction == Direction_Left)
    {
        gridOffset = TILE_SIZE - (((ray->start.x / TILE_SIZE) - (i32) (ray->start.x / TILE_SIZE)) * TILE_SIZE);
    }
    if (direction == Direction_Right)
    {
        gridOffset = ((ray->start.x / TILE_SIZE) - (i32) (ray->start.x / TILE_SIZE)) * TILE_SIZE;
    }

    while (t < maxDist)
    {
        Vector2 current = Vector2Add(ray->start, Vector2Scale(dir, t));

        i32 tileX = (i32) (current.x / TILE_SIZE);
        i32 tileY = (i32) (current.y / TILE_SIZE);

        if (tileX >= 0 && tileX < level->width && tileY >= 0 && tileY < level->height)
        {
            if (level->tiles[tileX + tileY * level->width] < Tile_Walls) {
                ray->end = Vector2Add(ray->start, Vector2Scale(dir, t - gridOffset));
                return t - gridOffset;
            }
        }

        t += TILE_SIZE;
    }

    ray->end = Vector2Add(ray->start, Vector2Scale(dir, maxDist));

    return 100 * TILE_SIZE;
}

void UpdatePlayer(Player *player, Level *level, float delta)
{

    player->vx = 0;
    
    if (IsKeyDown(KEY_A)) 
    {
        player->vx -= PLAYER_HOR_SPD * delta;
    }
    if (IsKeyDown(KEY_D)) 
    {
        player->vx += PLAYER_HOR_SPD * delta;
    }

    player->vy += G * delta;

    if (player->canJump && IsKeyPressed(KEY_SPACE))
    {
        player->canJump = false;
        player->vy = PLAYER_JUMP_SPD;
    }

    if (player->vx > 0)
    {
        f32 right = Min(
            Raycast(player, level, { TILE_SIZE / 2, 0.2 * TILE_SIZE }, Direction_Right),
            Raycast(player, level, { TILE_SIZE / 2, 0.8 * TILE_SIZE }, Direction_Right),
            Raycast(player, level, { TILE_SIZE / 2, 1.55 * TILE_SIZE }, Direction_Right)
        );

        if (right < player->vx)
        {
            player->vx = 0;
            player->position.x += right;
        }
        else
        {
            player->position.x += player->vx;
        }
    } 
    else 
    {
        f32 left = Min(
            Raycast(player, level, { -TILE_SIZE / 2, 0.2 * TILE_SIZE }, Direction_Left),
            Raycast(player, level, { -TILE_SIZE / 2, 0.8 * TILE_SIZE }, Direction_Left),
            Raycast(player, level, { -TILE_SIZE / 2, 1.55 * TILE_SIZE }, Direction_Left)
        );

        if (-left > player->vx)
        {
            player->vx = 0;
            player->position.x += -left;
        }
        else
        {
            player->position.x += player->vx;
        }
    }

    if (player->vy < 0)
    {
        f32 up = -Min(
            Raycast(player, level, {-0.9 * TILE_SIZE / 2, 0}, Direction_Up),
            Raycast(player, level, {0.9 * TILE_SIZE / 2, 0}, Direction_Up)
        );

        if (up > 0 || player->vy * delta < up)
        {
            player->vy = 0;
            player->position.y += up;
        }
        else
        {
            player->position.y += player->vy * delta;
        }
    }
    else
    {
        f32 down = Min(
            Raycast(player, level, { -0.9 * TILE_SIZE / 2, PLAYER_HEIGHT * TILE_SIZE }, Direction_Down),
            Raycast(player, level, { 0.9 * TILE_SIZE / 2, PLAYER_HEIGHT * TILE_SIZE }, Direction_Down)
        );

        if (down < 7 || player->vy * delta > down)
        {
            player->vy = 0;
            player->position.y += down;
            player->canJump = true;
        }
        else
        {
            player->position.y += player->vy * delta;
            player->canJump = false;
        }
    }
    
    // animation

    u32 old_state = player->state;

    if (player->vx < 0) {
        player->state = RunLeft;
    }
    else if (player->vx > 0) {
        player->state = RunRight;
    }
    else {
        if (player->state == RunLeft){
            player->state = IdleLeft;
        }
        if (player->state == RunRight){
            player->state = IdleRight;
        }
    }

    if (player->state != old_state) {
        player->animation_frame = 0;
    } else {
        player->animation_frame++;
    }

    //Spike Collision

    for(u32 i= 0; i < level->spike_count;i++){
        Spikes spike = level->spikes[i];
        Vector2 left_corner, right_corner, peak;

        if (spike.rotation == 0){
            left_corner = {spike.position.x, spike.position.y + TILE_SIZE };
            right_corner = {spike.position.x + TILE_SIZE, spike.position.y + TILE_SIZE };
            peak = {spike.position.x + TILE_SIZE/2 , spike.position.y };
        } else if(spike.rotation == 1){
            left_corner = {spike.position.x, spike.position.y };
            right_corner = {spike.position.x, spike.position.y + TILE_SIZE };
            peak = {spike.position.x + TILE_SIZE , spike.position.y + TILE_SIZE/2};
        } else if(spike.rotation == 2){
            left_corner = {spike.position.x + TILE_SIZE, spike.position.y };
            right_corner = {spike.position.x, spike.position.y };
            peak = {spike.position.x + TILE_SIZE/2 , spike.position.y + TILE_SIZE};
        } else{
            left_corner = {spike.position.x + TILE_SIZE, spike.position.y + TILE_SIZE };
            right_corner = {spike.position.x + TILE_SIZE, spike.position.y };
            peak = {spike.position.x , spike.position.y + TILE_SIZE/2};

        }

        Vector2 player_pos = {player->position.x - TILE_SIZE/2, player->position.y};
        Vector2 player_size = {TILE_SIZE, PLAYER_HEIGHT * TILE_SIZE};

        if(aabb_contains_point(player_pos, player_size, left_corner) ||
           aabb_contains_point(player_pos, player_size, right_corner) ||
           aabb_contains_point(player_pos, player_size, peak))
        {
            reset_level = true;
        }
    }


}

void UpdateParticles(Game *game, f32 delta)
{
    // Rain...
    for (u32 i = 0; i < 2; ++i)
    {
        for (u32 j = 0; j < RAIN_PARTICLE_COUNT; ++j)
        {
            Vector2 position = game->rainPosition[i][j];
            position.y += rainVelocity[i] * delta;
            
            if (position.y > game->framebufferSize[1])
            {
                position.x = Halton(i * RAIN_PARTICLE_COUNT + j, 2) * game->framebufferSize[0];
                position.y = 0;
            }

            game->rainPosition[i][j] = position;
        }
    }

    game->timeUntilNextFlash -= delta;
    game->flashIntensity -= delta;

    if (game->timeUntilNextFlash < 0)
    {
        game->flashIntensity = 0.3;
        game->timeUntilNextFlash = NextHalton2() * 6 + 1;
    }
}

i32 main(void)
{
    SetTraceLogLevel(LOG_DEBUG);

    i32 width = 1600;
    i32 height = 920;

    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(width, height, "Synchronize");

    Shader postprocess = LoadShader(0, "shader/postprocess.glsl");
    i32 postprocessSizeLoc = GetShaderLocation(postprocess, "size");

    LoadAssets();
    InitializeSamplers();
    PopulateTileRuleLookup();

    current_level = 0;
    reset_level = false;

    Game game = {};
    LoadGameFromFile(&game, level_order[current_level]);

    SetTargetFPS(60);

    f32 time = 0;

    while (!WindowShouldClose())
    {
        f32 delta = GetFrameTime();
        time += delta;

        ray_count = 0;

        if (IsKeyPressed(KEY_N))
        {
            current_level = (current_level + 1) % TOTAL_LEVEL_COUNT;
            reset_level = true;
        }

        if (IsKeyPressed(KEY_R))
        {   
            reset_level = true;
        }

        //Check goal
        u32 achieved_goals = 0;
        for (u32 i = 0; i < 2; ++i)
        {
            Vector2 player_position = {game.player[i].position.x - (TILE_SIZE/2.0f) , game.player[i].position.y};
            Vector2 player_size = {TILE_SIZE, PLAYER_HEIGHT * TILE_SIZE};
            for(u32 j = 0; j < game.level[i].goal_count; ++j)
            {
                if(aabb_intersects_aabb(game.level[i].goals[j].position, {TILE_SIZE, TILE_SIZE} ,
                                    player_position, player_size))
                {
                    achieved_goals++;
                    break;
                }
            }
        }
        if (achieved_goals == 2 && !reset_level)
        {
            current_level = (current_level + 1) % TOTAL_LEVEL_COUNT;
            reset_level = true;
        }


        if (reset_level)
        {
            LoadGameFromFile(&game, level_order[current_level]);
            reset_level = false;
        }

        if (!game.framebufferValid)
        {
            UnloadRenderTexture(game.framebuffer[0]);
            UnloadRenderTexture(game.framebuffer[1]);

            for (u32 i = 0; i < 2; ++i)
            {
                if (game.horizontal_split)
                {
                    game.framebuffer[i] = LoadRenderTexture(width, height / 2);
                    game.camera[i] = {};
                    game.camera[i].zoom = 1.0f;
                    game.camera[i].offset = { width / 2.0f, height / 4.0f };

                    game.framebufferSize[0] = width;
                    game.framebufferSize[1] = height / 2;
                }
                else
                {
                    game.framebuffer[i] = LoadRenderTexture(width / 2, height);
                    game.camera[i] = {};
                    game.camera[i].zoom = 1.0f;
                    game.camera[i].offset = { width / 4.0f, height / 2.0f };

                    game.framebufferSize[0] = width / 2;
                    game.framebufferSize[1] = height;
                }
            }

            SetShaderValue(postprocess, postprocessSizeLoc, game.framebufferSize, SHADER_UNIFORM_VEC2);

            // Recalculate rain particle positions
            for (u32 i = 0; i < 2; ++i)
            {
                for (u32 j = 0; j < RAIN_PARTICLE_COUNT; ++j)
                {
                    Vector2 position;
                    position.x = game.framebufferSize[0] * Halton(i * RAIN_PARTICLE_COUNT + j, 2);
                    position.y = game.framebufferSize[1] * Halton(i * RAIN_PARTICLE_COUNT + j, 3);
                    game.rainPosition[i][j] = position;
                }
            }

            game.framebufferValid = true;
        }

        for (u32 i = 0; i < 2; ++i)
        {
            Camera2D *camera = &game.camera[i];
            Level *level = &game.level[i];
            Player *player = &game.player[i];

            UpdatePlayer(player, level, delta);

            UpdateParticles(&game, 1.0f / 60.0f);

            //Synchronizer Update
            for(u32 j = 0; j < level->synchronizer_count; j++){
                if(aabb_intersects_aabb(level->synchronizers[j].position, {TILE_SIZE, TILE_SIZE} ,
                                    {player->position.x - (TILE_SIZE/2.0f) , player->position.y}, 
                                    {TILE_SIZE, PLAYER_HEIGHT * TILE_SIZE}))
                {
                    bool can_teleport = true;

                    // u32 int_x_pos = (int)((player->position.x - TILE_SIZE/2)/ TILE_SIZE);
                    // u32 int_y_pos = (int)((player->position.y - TILE_SIZE * PLAYER_HEIGHT )  / TILE_SIZE);
                    // if(0<= int_x_pos && int_x_pos < level->width && 
                    //    0 <= int_y_pos && int_y_pos < level->height){
                    //     u32 tile = level->tiles[int_x_pos + int_y_pos * level->width];
                    //     if(tile < Tile_Walls){
                    //         reset_level = true;
                    //     }
                    // }
                    // int_x_pos = (int)((player->position.x + TILE_SIZE/2)/ TILE_SIZE);
                    // int_y_pos = (int)((player->position.y - TILE_SIZE * PLAYER_HEIGHT )  / TILE_SIZE);
                    // if(0<= int_x_pos && int_x_pos < level->width && 
                    //    0 <= int_y_pos && int_y_pos < level->height){
                    //     u32 tile = level->tiles[int_x_pos + int_y_pos * level->width];
                    //     if(tile < Tile_Walls){
                    //         reset_level = true;
                    //     }
                    // }
                    // int_x_pos = (int)((player->position.x - TILE_SIZE/2)/ TILE_SIZE);
                    // int_y_pos = (int)((player->position.y)  / TILE_SIZE);
                    // if(0<= int_x_pos && int_x_pos < level->width && 
                    //    0 <= int_y_pos && int_y_pos < level->height){
                    //     u32 tile = level->tiles[int_x_pos + int_y_pos * level->width];
                    //     if(tile < Tile_Walls){
                    //         reset_level = true;
                    //     }
                    // }
                    // int_x_pos = (int)((player->position.x + TILE_SIZE/2)/ TILE_SIZE);
                    // int_y_pos = (int)((player->position.y)  / TILE_SIZE);
                    // if(0<= int_x_pos && int_x_pos < level->width && 
                    //    0 <= int_y_pos && int_y_pos < level->height){
                    //     u32 tile = level->tiles[int_x_pos + int_y_pos * level->width];
                    //     if(tile < Tile_Walls) {
                    //         reset_level = true;
                    //     }
                    // }

                    if(can_teleport){
                        player->position.x = game.player[1-i].position.x;
                        player->position.y = game.player[1-i].position.y;
                        player->vx = game.player[1-i].vx;
                        player->vy = game.player[1-i].vy;
                        player->canJump = game.player[1-i].canJump;
                        player->state = game.player[1-i].state;
                        player->animation_frame = game.player[1-i].animation_frame;
                    }
                    

                }
            }

            // End Synchronizer Update

            if (level->fixed_camera) {
                camera->target = level->fixed_camera_pos;
            } else {
                camera->target = Vector2Lerp(player->position, game.player[0].position, 0);
            }

            BeginTextureMode(game.framebuffer[i]);
            ClearBackground(clearColors[i]);
            BeginMode2D(*camera);

            // Render stars and rain...

            if (i == 0)
            {
                for (u32 j = 0; j < STAR_COUNT; ++j)
                {
                    Star star = game.star[j];
                    f32 x = star.x * game.framebufferSize[0];
                    f32 y = star.y * game.framebufferSize[1];
                    f32 intensity = Min(Max(sin(time * star.frequency) + 0.1, 0), 1);
                    DrawCircle(x, y, 1 + star.size * 3, {255, 255, 255, (u8) (intensity * 255)});
                }
            }
            else
            {
                for (u32 j = 0; j < 2; ++j)
                {
                    for (u32 k = 0; k < RAIN_PARTICLE_COUNT; ++k)
                    {
                        DrawCircleV(game.rainPosition[j][k], 3, rainColors[j]);
                    }
                }

                if (game.flashIntensity > 0)
                {
                    Color flashColor = WHITE;
                    flashColor.a = (u8) (255 * game.flashIntensity);
                    DrawRectangleV({}, {game.framebufferSize[0], game.framebufferSize[1]}, flashColor);
                }
            }

            // Render Tiles

            i32 padding = 5;
            for (i32 x = -padding; x < level->width + padding; ++x)
            {
                for (i32 y = -padding; y < level->height + padding; ++y)
                {
                    if (x < 0 || y < 0 || x >= level->width || y >= level->height)
                    {
                        DrawTextureRec(tileset, tile[i][Tile_WallFull], {x * TILE_SIZE, y * TILE_SIZE}, WHITE);
                        continue;
                    }

                    u32 type = level->tiles[x + y * level->width];

                    if (type < Tile_Walls) 
                    {
                        DrawTextureRec(tileset, tile[i][type], {x * TILE_SIZE, y * TILE_SIZE}, WHITE);
                    }
                }
            }

            // Render puddles
            
            for (u32 j = 0; j < level->puddle_count; ++j)
            {
                DrawTextureRec(tileset, puddle[0], {x * TILE_SIZE, y * TILE_SIZE}, WHITE);
            }

            for (i32 j = 0; j < level->spike_count; ++j)
            {
                DrawTextureRec(tileset, spike[i][level->spikes[j].rotation], level->spikes[j].position, WHITE);
            }

            for (i32 j = 0; j < level->goal_count; ++j)
            {
                Rectangle tile = { level->goals[j].position.x, level->goals[j].position.y, TILE_SIZE, TILE_SIZE };
                DrawRectangleRec(tile, YELLOW);
            }
            
            for (i32 j = 0; j < level->synchronizer_count; ++j)
            {
                Rectangle tile = { level->synchronizers[j].position.x, level->synchronizers[j].position.y, TILE_SIZE, TILE_SIZE };
                DrawRectangleRec(tile, GREEN);
            }

            // Render player

            // Rectangle playerRect = { player->position.x - TILE_SIZE / 2, player->position.y, TILE_SIZE, PLAYER_HEIGHT * TILE_SIZE };
            // DrawRectangleRec(playerRect, GREEN);

            Vector2 player_render_position = {player->position.x - (TILE_SIZE *0.5f), player->position.y  };
            bool player_is_blue = (i == 0);

            switch(player->state){
                case IdleLeft : 
                    player_frame_rec.y = 2 * player_sprite_texture.height / 4;
                    player_frame_rec.x = ((player->animation_frame / 20) % 2) * player_sprite_texture.width / 6;
                    if (!player_is_blue){
                        player_frame_rec.x += 2 * player_sprite_texture.width / 6;
                    }
                    DrawTextureRec(player_sprite_texture, player_frame_rec, player_render_position, WHITE); break;
                case IdleRight : 
                    player_frame_rec.y = 0 * player_sprite_texture.height / 4;
                    player_frame_rec.x = ((player->animation_frame / 20) % 2) * player_sprite_texture.width / 6;
                    if (!player_is_blue){
                        player_frame_rec.x += 2 * player_sprite_texture.width / 6;
                    }
                    DrawTextureRec(player_sprite_texture, player_frame_rec, player_render_position, WHITE); break;
                case RunLeft : 
                    player_frame_rec.y = 3 * player_sprite_texture.height / 4;
                    player_frame_rec.x = ((player->animation_frame / 5) % 3) * player_sprite_texture.width / 6;
                    if (!player_is_blue){
                        player_frame_rec.x += 3 * player_sprite_texture.width / 6;
                    }
                    DrawTextureRec(player_sprite_texture, player_frame_rec, player_render_position, WHITE); break;
                case RunRight : 
                    player_frame_rec.y = 1 * player_sprite_texture.height / 4;
                    player_frame_rec.x = ((player->animation_frame / 5) % 3) * player_sprite_texture.width / 6;
                    if (!player_is_blue){
                        player_frame_rec.x += 3 * player_sprite_texture.width / 6;
                    }
                    DrawTextureRec(player_sprite_texture, player_frame_rec, player_render_position, WHITE); break;
            }

            // End Render Player

#if 0
            for (u32 i = 0; i < ray_count; ++i) 
            {
                DebugRay ray = rays[i];
                DrawLineV(ray.start, ray.end, ray.color);
            }
#endif

            EndMode2D();
            EndTextureMode();
        }


        BeginDrawing();

        ClearBackground(BLACK);

        Rectangle textureRect = {0.0f, 0.0f, (i32) game.framebufferSize[0], (i32) -game.framebufferSize[1]};
        if (game.horizontal_split)
        {
            BeginShaderMode(postprocess);
            DrawTextureRec(game.framebuffer[0].texture, textureRect, { 0, 0 }, WHITE);
            DrawTextureRec(game.framebuffer[1].texture, textureRect, { 0, height / 2.0f }, WHITE);
            EndShaderMode();
        }
        else
        {
            BeginShaderMode(postprocess);
            DrawTextureRec(game.framebuffer[0].texture, textureRect, { 0, 0 }, WHITE);
            DrawTextureRec(game.framebuffer[1].texture, textureRect, { width / 2.0f, 0 }, WHITE);
            EndShaderMode();
        }

        // TODO:
        // DrawRectangle(GetScreenWidth()/2 - 2, 0, 4, GetScreenHeight(), LIGHTGRAY);

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
