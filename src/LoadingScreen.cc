#include "LoadingScreen.hh"

LoadingScreen::LoadingScreen()
{
    _sprLoadingScreen = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_LOADING_SCREEN).c_str());
}

LoadingScreen::~LoadingScreen()
{
    UnloadTexture(_sprLoadingScreen);
}

void LoadingScreen::DrawLoadingScreen(const int width, const int height)
{
    int horizontalBackgroundTiles = (width / LOADING_SCREEN_SPRITE_SIZE) + 1;
    int verticalBackgroundTiles = (height / LOADING_SCREEN_SPRITE_SIZE) + 1;

    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < horizontalBackgroundTiles; ++i)
    {
        for (int j = 0; j < verticalBackgroundTiles; ++j)
        {
            DrawTexturePro(
                _sprLoadingScreen,
                { LOADING_SCREEN_SPRITE_SIZE, 0, LOADING_SCREEN_SPRITE_SIZE, LOADING_SCREEN_SPRITE_SIZE },
                { (float)(i * LOADING_SCREEN_SPRITE_SIZE), (float)(j * LOADING_SCREEN_SPRITE_SIZE),
                  LOADING_SCREEN_SPRITE_SIZE, LOADING_SCREEN_SPRITE_SIZE },
                { 0, 0 }, 0.0f, WHITE
            );
        }
    }

    DrawTexturePro(
        _sprLoadingScreen,
        { 0, 0, LOADING_SCREEN_SPRITE_SIZE, LOADING_SCREEN_SPRITE_SIZE },
        { (width - LOADING_SCREEN_SPRITE_SIZE) / 2.0f,
          (height - LOADING_SCREEN_SPRITE_SIZE) / 2.0f,
          LOADING_SCREEN_SPRITE_SIZE, LOADING_SCREEN_SPRITE_SIZE },
        { 0, 0 }, 0.0f, WHITE
    );

    EndDrawing();
}