#include "GameManager.hh"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
{
    GameManager::instance = this;

    _gameObjects = vector<GameObject*>();

    timeToSpawnNextBomb = BOMB_SPAWN_TIME;
}

void GameManager::Update(float deltaTime)
{
    timeToSpawnNextBomb -= deltaTime;
    if (timeToSpawnNextBomb <= 0) {
        timeToSpawnNextBomb = BOMB_SPAWN_TIME;

        int spawnPos = rand() % 2;
        if (spawnPos == 1) {
            InstantiateObject(new Bomb({250,0}, 0, 20));
        } else {
            InstantiateObject(new Bomb({250,500}, 0, 20));
        }
    }

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

void GameManager::InstantiateObject(GameObject* obj) 
{
    _gameObjects.push_back(obj);
}

void GameManager::DestroyObject(GameObject* obj) 
{
    
}