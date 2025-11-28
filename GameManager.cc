#include "GameManager.hh"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
{
    GameManager::instance = this;

    StartGame();
}

void GameManager::StartGame()
{
    _gameObjects = vector<GameObject*>();

    timeToSpawnNextBomb = BOMB_SPAWN_TIME_START;

    _bombHouseLeft = new BombHouse({0,200},0,1, BOMBHOUSE_LEFT);
    _bombHouseRight = new BombHouse({400,200},0,1, BOMBHOUSE_RIGHT);
    InstantiateObject(_bombHouseLeft);
    InstantiateObject(_bombHouseRight);

    didGameOver = false;
}

void GameManager::Update(float deltaTime)
{
    if (didGameOver) {
        if (IsKeyPressed(KEY_R)) {
            StartGame();
            return;
        }
    }

    timeToSpawnNextBomb -= deltaTime;
    if (timeToSpawnNextBomb <= 0 && !didGameOver) {
        bombSpawnTime = max(bombSpawnTime - 0.25f, 1f);
        timeToSpawnNextBomb = bombSpawnTime;

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

    if (didGameOver) DrawText("Has mort :)", 190, 100, 20, RED);

    for (GameObject* o : _gameObjects) {
        o->Render(deltaTime);
    }

    EndDrawing();
}

void GameManager::GameOver()
{
    didGameOver = true;

    for (GameObject* o : _gameObjects) {
        o->GameOver();
    }
}

void GameManager::InstantiateObject(GameObject* obj) 
{
    _gameObjects.push_back(obj);
}

void GameManager::DestroyObject(GameObject* obj) 
{
    
}

int GameManager::GetBombReleasedState(Bomb* obj)
{
    BombType type = obj->GetType();

    if (_bombHouseLeft->GetIsMouseInside(obj)) {
        return (type == BOMB_LEFT) ? 1 : -1;
    }
    if (_bombHouseRight->GetIsMouseInside(obj)) {
        return (type == BOMB_RIGHT) ? 2 : -1;
    }

    return 3;
}