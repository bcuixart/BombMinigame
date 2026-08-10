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
    const int SIZE = LOADING_SCREEN_SPRITE_SIZE;

    int tilesH = (width / SIZE) + 3;
    int tilesV = (height / SIZE) + 3;

    int centerTileX = tilesH / 2;
    int centerTileY = tilesV / 2;

    float offsetX = (width - SIZE) / 2.0f - centerTileX * SIZE;
    float offsetY = (height - SIZE) / 2.0f - centerTileY * SIZE;

    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < tilesH; ++i)
    {
        for (int j = 0; j < tilesV; ++j)
        {
            bool isCenter = (i == centerTileX && j == centerTileY);
            float srcX = isCenter ? 0.0f : (float)SIZE;

            DrawTexturePro(
                _sprLoadingScreen,
                { srcX, 0, (float)SIZE, (float)SIZE },
                { offsetX + i * SIZE, offsetY + j * SIZE, (float)SIZE, (float)SIZE },
                { 0, 0 }, 0.0f, WHITE
            );
        }
    }

    EndDrawing();
}