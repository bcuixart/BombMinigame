#include <raylib.h>

#include "GameManager.hh"
#include "LoadingScreen.hh"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

int main(int argc, char* argv[])
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    if (argc == 3) 
    {
		int wdth = atoi(argv[1]);
		int hght = atoi(argv[2]);

        InitWindow(wdth, hght, "Boom Boom Workshop");
    }
    else 
    {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Boom Boom Workshop");
    }

    SetExitKey(KEY_NULL);

    InitAudioDevice();
    SetTargetFPS(30);

	LoadingScreen* loadingScreen = new LoadingScreen();
	loadingScreen->DrawLoadingScreen(GetScreenWidth(), GetScreenHeight());

    GameManager* gameManager = new GameManager();

	delete loadingScreen;

    float deltaTime = 0;
    while (!WindowShouldClose() && !gameManager->PlayerExited())
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
