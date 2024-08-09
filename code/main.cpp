#include "raylib.h"

#include "game.h"


#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f
#define TILE_SIZE 32

#include "loader.cpp"

f32 Raycast(Player *player, Level *level, Vector2 offset, Vector2 direction)
{
}

void UpdatePlayer(Player *player, float delta)
{
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD * delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD * delta;
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

    u32 buffer[1024];

    Level level = load_from_file(1);
    level.width = 20;
    level.height = 20;
    level.tiles = buffer;

    for (u32 x = 0; x < level.width; ++x) 
    {
        for (u32 y = 0; y < level.height; ++y) 
        {
            if (x == 0 || y == 0 || y == (level.height - 1))
            {
                level.tiles[x + y * level.width] = 1;
                continue;
            }
            level.tiles[x + y * level.width] = 0;
        }
    }

    Camera2D camera = {};
    camera.offset = { width / 2.0f, height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();

        UpdatePlayer(&player, delta);

        camera.target = { player.position.x, player.position.y };

        BeginDrawing();
        ClearBackground(WHITE);

        BeginMode2D(camera);

        for (i32 x = 0; x < level.width; ++x)
        {
            for (i32 y = 0; y < level.height; ++y)
            {
                u32 type = level.tiles[x + y * level.width];
                if (type) 
                {
                    Rectangle tile = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                    DrawRectangleRec(tile, BLUE);
                }
            }
        }

        Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40.0f, 40.0f };
        DrawRectangleRec(playerRect, RED);

        EndMode2D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

