#include <raylib.h>

#include "GameManager.hh"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BOMB GAME");

    GameManager* gameManager = new GameManager();

    float deltaTime = 0;
    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();

        gameManager->Update(deltaTime);
        gameManager->Render(deltaTime);
    }

    delete gameManager;

    CloseWindow();

    return 0;
}
