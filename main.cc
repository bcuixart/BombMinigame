#include <raylib.h>

#include "GameManager.hh"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

int main(int argc, char* argv[])
{
    if (argc == 3) 
    {
		int wdth = atoi(argv[1]);
		int hght = atoi(argv[2]);

        InitWindow(wdth, hght, "BOMB GAME");
    }
    else 
    {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BOMB GAME");
    }

    InitAudioDevice();
    SetTargetFPS(30);

    GameManager* gameManager = new GameManager();

    float deltaTime = 0;
    while (!WindowShouldClose()) 
    {
        deltaTime = GetFrameTime();

        gameManager->Update(deltaTime);
        gameManager->Render(deltaTime);
    }

    delete gameManager;

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
