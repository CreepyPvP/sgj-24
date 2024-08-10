#include "raylib.h"

#include "game.h"
#include "loader.h"

#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define TILE_SIZE 32.0f

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

f32 Raycast(Player *player, Level *level, Vector2 offset, Vector2 direction)
{
    f32 maxDist = 200;

    DebugRay *ray = AllocRay();
    ray->start = player->position;
    ray->color = BLACK;

    f32 t = 0;

    while (t < maxDist)
    {
        Vector2 current = Vector2Add(player->position, Vector2Scale(direction, t));

        i32 tileX = (i32) current.x;
        i32 tileY = (i32) current.y;

        if (tileX >= 0 && tileX < level->width && tileY >= 0 && tileY < level->height)
        {
            if (level->tiles[tileX + tileY * level->width]) {
                ray->end = Vector2Add(player->position, Vector2Scale(direction, t));
                return t;
            }
        }

        t++;
    }

    ray->end = Vector2Add(player->position, Vector2Scale(direction, maxDist));

    return -1;
}

void UpdatePlayer(Player *player, Level *level, float delta)
{
    if (IsKeyDown(KEY_LEFT)) 
    {
        player->position.x -= PLAYER_HOR_SPD * delta;
    }
    if (IsKeyDown(KEY_RIGHT)) 
    {
        player->position.x += PLAYER_HOR_SPD * delta;
    }
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }

    bool hitObstacle = false;

    if (!hitObstacle)
    {
        player->position.y += player->speed * delta;
        player->speed += G * delta;
        player->canJump = false;
    }
    else 
    {
        player->canJump = true;
    }

    Raycast(player, level, {0, 0}, {0, 1});
    // Raycast(player, level, {0, 0}, {1, 0});
}

i32 main(void)
{
    i32 width = 1600;
    i32 height = 920;

    InitWindow(width, height, "Synchronize");

    Player player = {};
    player.position = { 400, 280 };
    player.speed = 0;
    player.canJump = false;

    u32 buffer1[2048];
    u32 buffer2[2048];
    Game game = LoadGameFromFile(1, buffer1, buffer2);
    Level level = game.level[0];

    Camera2D camera = {};
    camera.offset = { width / 2.0f, height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();

        ray_count = 0;

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
                    DrawRectangleRec(tile, BLUE);
                }

                if( type == Tile_Player){
                    player.position = {x * TILE_SIZE + (TILE_SIZE/2), y * TILE_SIZE - (TILE_SIZE/2)};
                    level.tiles[x + y * level.width] = Tile_Air;
                }

                if (type == Tile_Spikes) 
                {
                    Rectangle tile = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                    DrawRectangleRec(tile, RED);
                }

                if (type == Tile_Goal) 
                {
                    Rectangle tile = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                    DrawRectangleRec(tile, YELLOW);
                }


            }
        }

        for (u32 i = 0; i < ray_count; ++i) 
        {
            DebugRay ray = rays[i];
            DrawLineV(ray.start, ray.end, ray.color);
        }

        Rectangle playerRect = { player.position.x - TILE_SIZE / 2, player.position.y, TILE_SIZE, 1.75 * TILE_SIZE };
        DrawRectangleRec(playerRect, GREEN);

        EndMode2D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

