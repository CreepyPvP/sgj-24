#include "raylib.h"
#include "game.h"
#include "loader.h"
#include "game_math.h"

#include <cassert>
#include <stdio.h>

#define G 1500.0f
#define PLAYER_JUMP_SPD -800.0f
#define PLAYER_HOR_SPD 600.0f

#define TOTAL_LEVEL_COUNT 3

u32 current_level;


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

Texture2D tile1_texture;
Texture2D tile2_texture;
Texture2D spike1_texture;
Texture2D spike2_texture;

Texture2D idle_left_texture;
Texture2D idle_right_texture;
Texture2D run_left_texture;
Texture2D run_right_texture;

Rectangle idle_frame_rec;
Rectangle run_frame_rec;

void LoadAssets(){
    Image tile1_image = LoadImage("assets/tiles1.png");
    ImageResize(&tile1_image,64,64);
    tile1_texture = LoadTextureFromImage(tile1_image);
    UnloadImage(tile1_image);
    assert(tile1_texture.id != 0);

    Image tile2_image = LoadImage("assets/tiles2.png");
    ImageResize(&tile2_image,64,64);
    tile2_texture = LoadTextureFromImage(tile2_image);
    UnloadImage(tile2_image);
    assert(tile2_texture.id != 0);
    
    Image spike1_image = LoadImage("assets/spike1.png");
    ImageResize(&spike1_image,64,64);
    spike1_texture = LoadTextureFromImage(spike1_image);
    UnloadImage(spike1_image);
    assert(spike1_texture.id != 0);
    
    Image spike2_image = LoadImage("assets/spike2.png");
    ImageResize(&spike2_image,64,64);
    spike2_texture = LoadTextureFromImage(spike2_image);
    UnloadImage(spike2_image);
    assert(spike2_texture.id != 0);

    Image idle_sheet_image = LoadImage("assets/player/IdleSheet.png");
    ImageResize(&idle_sheet_image,160,160);
    idle_right_texture = LoadTextureFromImage(idle_sheet_image);
    ImageFlipHorizontal(&idle_sheet_image);
    idle_left_texture = LoadTextureFromImage(idle_sheet_image);
    UnloadImage(idle_sheet_image);
    assert(idle_left_texture.id != 0);

    
    Image run_sheet_image = LoadImage("assets/player/RunSheet.png");
    ImageResize(&run_sheet_image,240,160);
    run_right_texture = LoadTextureFromImage(run_sheet_image);
    ImageFlipHorizontal(&run_sheet_image);
    run_left_texture = LoadTextureFromImage(run_sheet_image);
    UnloadImage(run_sheet_image);
    assert(run_left_texture.id != 0);

    idle_frame_rec = { 0.0f, 0.0f, (float)idle_right_texture.width/2, (float)idle_right_texture.height };
    run_frame_rec = { 0.0f, 0.0f, (float)run_right_texture.width/3, (float)run_right_texture.height };
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
            if (level->tiles[tileX + tileY * level->width]) {
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
            player->position.x += player->vx * delta;
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
            player->position.x += player->vx * delta;
        }
    }
    player->position.x += player->vx;

    if (player->vy < 0)
    {
        f32 up = -Min(
            Raycast(player, level, {-0.9 * TILE_SIZE / 2, 0}, Direction_Up),
            Raycast(player, level, {0.9 * TILE_SIZE / 2, 0}, Direction_Up)
        );

        if (up > 0)
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
            Raycast(player, level, { -0.9 * TILE_SIZE / 2, 1.75 * TILE_SIZE }, Direction_Down),
            Raycast(player, level, { 0.9 * TILE_SIZE / 2, 1.75 * TILE_SIZE }, Direction_Down)
        );

        if (down < 7)
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
    player->position.y += player->vy;






    u32 old_state = player->state;

    if(player->vx < 0){
        player->state = RunLeft;
    }
    else if(player->vx > 0){
        player->state = RunRight;
    }
    else{
        if (player->state == RunLeft){
            player->state = IdleLeft;
        }
        if (player->state == RunRight){
            player->state = IdleRight;
        }
    }

    if(player->state != old_state){
        player->animation_frame = 0;
    }else{
        player->animation_frame++;
    }
}

i32 main(void)
{
    SetTraceLogLevel(LOG_DEBUG);

    i32 width = 1600;
    i32 height = 920;

    InitWindow(width, height, "Synchronize");

    LoadAssets();

    current_level = 0;

    Game game = {};
    LoadGameFromFile(&game, current_level);

    SetTargetFPS(60);

    // Texture2D texture = LoadTexture("assets/tiles.png");

    while (!WindowShouldClose())
    {
        f32 delta = GetFrameTime();
        ray_count = 0;

        if (IsKeyPressed(KEY_N))
        {
            current_level = (current_level + 1) % TOTAL_LEVEL_COUNT;
            LoadGameFromFile(&game, current_level);
        }

        if (IsKeyPressed(KEY_R))
        {
            LoadGameFromFile(&game, current_level);
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
                }
                else
                {
                    game.framebuffer[i] = LoadRenderTexture(width / 2, height);
                    game.camera[i] = {};
                    game.camera[i].zoom = 1.0f;
                    game.camera[i].offset = { width / 4.0f, height / 2.0f };
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
            camera->target = { player->position.x, player->position.y };

            BeginTextureMode(game.framebuffer[i]);
            ClearBackground(i == 0? RAYWHITE : SKYBLUE);
            BeginMode2D(*camera);

            for (i32 i = 0; i < level->spike_count; ++i)
            {
                Rectangle tile = { level->spikes[i].position.x, level->spikes[i].position.y, TILE_SIZE, TILE_SIZE };
                DrawTexture(spike1_texture, level->spikes[i].position.x, level->spikes[i].position.y, WHITE);
            }

            for (i32 i = 0; i < level->goal_count; ++i)
            {
                Rectangle tile = { level->goals[i].position.x, level->goals[i].position.y, TILE_SIZE, TILE_SIZE };
                DrawRectangleRec(tile, YELLOW);
            }

            for (i32 x = 0; x < level->width; ++x)
            {
                for (i32 y = 0; y < level->height; ++y)
                {
                    u32 type = level->tiles[x + y * level->width];
                    if (type == Tile_Wall) 
                    {
                        Rectangle tile = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                        DrawTexture(tile1_texture, x * TILE_SIZE, y * TILE_SIZE, WHITE);
                    }
                }
            }

            Vector2 player_render_position = {player->position.x - (TILE_SIZE *0.625f), player->position.y - (TILE_SIZE / 2)  };

            Rectangle playerRect = { player->position.x - TILE_SIZE / 2, player->position.y, TILE_SIZE, 1.75 * TILE_SIZE };
            DrawRectangleRec(playerRect, GREEN);

            switch(player->state){
                case IdleLeft: 
                    idle_frame_rec.x = (player->animation_frame / 20) * idle_right_texture.width/2;
                    DrawTextureRec(idle_left_texture, idle_frame_rec, player_render_position, WHITE); break;
                case IdleRight: 
                    idle_frame_rec.x = (player->animation_frame / 20) * idle_right_texture.width/2;
                    DrawTextureRec(idle_right_texture, idle_frame_rec, player_render_position, WHITE); break;
                case RunLeft: 
                    run_frame_rec.x = (player->animation_frame / 5) * run_right_texture.width/3;
                    DrawTextureRec(run_left_texture, run_frame_rec, player_render_position, WHITE); break;
                case RunRight: 
                    run_frame_rec.x = (player->animation_frame / 5) * run_right_texture.width/3;
                    DrawTextureRec(run_right_texture, run_frame_rec, player_render_position, WHITE); break;
            }

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

        Rectangle textureRect = {0.0f, 0.0f, game.framebuffer[0].texture.width, -game.framebuffer[1].texture.height};
        if (game.horizontal_split)
        {
            DrawTextureRec(game.framebuffer[0].texture, textureRect, { 0, 0 }, WHITE);
            DrawTextureRec(game.framebuffer[1].texture, textureRect, { 0, height / 2.0f }, WHITE);
        }
        else
        {
            DrawTextureRec(game.framebuffer[0].texture, textureRect, { 0, 0 }, WHITE);
            DrawTextureRec(game.framebuffer[1].texture, textureRect, { width / 2.0f, 0 }, WHITE);
        }

        // TODO:
        // DrawRectangle(GetScreenWidth()/2 - 2, 0, 4, GetScreenHeight(), LIGHTGRAY);

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
