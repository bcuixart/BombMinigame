#include "GameManager.hh"

GameManager::GameManager()
{
    _gameObjects = vector<GameObject*>();
}

void GameManager::Update(float deltaTime)
{
    for (GameObject* o : _gameObjects) {
        o->Update(deltaTime);
    }
}

void GameManager::Render(const float deltaTime) 
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (GameObject* o : _gameObjects) {
        o->Render(deltaTime);
    }

    EndDrawing();
}