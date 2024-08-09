#include "raylib.h"
#include "raymath.h"

#include <stdint.h>

typedef int32_t i32;
typedef uint32_t u32;
typedef float f32;

#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

struct Player {
    Vector2 position;
    f32 speed;
    bool canJump;
};

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
        player->position.y += player->speed*delta;
        player->speed += G * delta;
        player->canJump = false;
    }
    else 
    {
        player->canJump = true;
    }
}

int main(void)
{
    i32 width = 1600;
    i32 height = 920;

    InitWindow(width, height, "Fuck");

    Player player = { 0 };
    player.position = { 400, 280 };
    player.speed = 0;
    player.canJump = false;

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = { width / 2.0f, height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();

        UpdatePlayer(&player, delta);

        BeginDrawing();
        ClearBackground(WHITE);
        BeginMode2D(camera);

        Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40.0f, 40.0f };
        DrawRectangleRec(playerRect, RED);

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

