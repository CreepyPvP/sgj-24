#include "raylib.h"
#include "game.h"
#include "loader.h"
#include "game_math.h"
#include <cassert>
#include <stdio.h>

#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

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
    f32 maxDist = 10 * TILE_SIZE;
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

    return -1;
}

void UpdatePlayer(Player *player, Level *level, float delta)
{
    if (IsKeyDown(KEY_W)) 
    {
        player->position.y -= PLAYER_HOR_SPD * delta;
    }
    if (IsKeyDown(KEY_S)) 
    {
        player->position.y += PLAYER_HOR_SPD * delta;
    }
    if (IsKeyDown(KEY_A)) 
    {
        player->position.x -= PLAYER_HOR_SPD * delta;
    }
    if (IsKeyDown(KEY_D)) 
    {
        player->position.x += PLAYER_HOR_SPD * delta;
    }

    float horizontal = 0;

    if (IsKeyDown(KEY_A)) 
    {
        horizontal -= PLAYER_HOR_SPD * delta;
    }
    if (IsKeyDown(KEY_D)) 
    {
        horizontal += PLAYER_HOR_SPD * delta;
    }

    if (horizontal > 0)
    {
        horizontal = Min(Raycast(player, level, { TILE_SIZE / 2, 0 }, Direction_Right), horizontal);
        player->position.x += horizontal;
    } 
    else 
    {
        horizontal = Max(-Raycast(player, level, { -TILE_SIZE / 2, 0}, Direction_Left), horizontal);
        player->position.x += horizontal;
    }

    // if (IsKeyDown(KEY_SPACE) && player->canJump)
    // {
    //     player->speed = -PLAYER_JUMP_SPD;
    //     player->canJump = false;
    // }

    // bool hitObstacle = false;
    //
    // if (!hitObstacle)
    // {
    //     player->position.y += player->speed * delta;
    //     player->speed += G * delta;
    //     player->canJump = false;
    // }
    // else 
    // {
    //     player->canJump = true;
    // }

    // Raycast(player, level, {0, 1.75 * TILE_SIZE}, {0, 1});
    // Raycast(player, level, {0, 0}, {1, 0});
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
    Level level = game.level[0];
    Player player = level.player;

    Camera2D camera = {};
    camera.offset = { width / 2.0f, height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    // Texture2D texture = LoadTexture("assets/tiles.png");

    while (!WindowShouldClose())
    {
        f32 delta = GetFrameTime();
        // float delta = 0;
        // if (IsKeyDown(KEY_W)) 
        // {
        //     delta = GetFrameTime();
        // }
        // if (IsKeyDown(KEY_S)) 
        // {
        //     delta = -GetFrameTime();
        // }

        ray_count = 0;

        if (IsKeyPressed(KEY_N)){
            current_level = (current_level + 1) % TOTAL_LEVEL_COUNT;
            LoadGameFromFile(&game, current_level);
            level = game.level[0];
            player = level.player;
        }

        UpdatePlayer(&player, &level, delta);
        camera.target = { player.position.x, player.position.y };

        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode2D(camera);

        for (i32 x = 0; x < level.width; ++x)
        {
            for (i32 y = 0; y < level.height; ++y)
            {
                u32 type = level.tiles[x + y * level.width];
                if (type == Tile_Wall) 
                {
                    Rectangle tile = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                    DrawTexture(tile1_texture, x * TILE_SIZE, y * TILE_SIZE, WHITE);
                }
            }
        }

        for (i32 i = 0; i < level.spike_count; ++i)
        {
            Rectangle tile = { level.spikes[i].position.x, level.spikes[i].position.y, TILE_SIZE, TILE_SIZE };
            DrawTexture(spike1_texture, level.spikes[i].position.x, level.spikes[i].position.y, WHITE);
        }

        for (i32 i = 0; i < level.goal_count; ++i)
        {
            Rectangle tile = { level.goals[i].position.x, level.goals[i].position.y, TILE_SIZE, TILE_SIZE };
            DrawRectangleRec(tile, YELLOW);
        }


        Rectangle playerRect = { player.position.x - TILE_SIZE / 2, player.position.y, TILE_SIZE, 1.75 * TILE_SIZE };
        DrawRectangleRec(playerRect, GREEN);

        for (u32 i = 0; i < ray_count; ++i) 
        {
            DebugRay ray = rays[i];
            DrawLineV(ray.start, ray.end, ray.color);
        }

        EndMode2D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

