#include <raylib.h>

#include "GameManager.hh"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BOMB GAME");
    SetTargetFPS(30);

    GameManager* gameManager = new GameManager();

    float deltaTime = 0;
    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();

        gameManager->Update(deltaTime);
        gameManager->Render(deltaTime);

        DrawFPS(10, 10);
    }

    delete gameManager;

    CloseWindow();

    return 0;
}
