#include "GameManager.hh"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
{
    GameManager::instance = this;

    StartGame();
}

void GameManager::StartGame()
{
    _bombGameObjects = vector<Bomb*>();

    timeToSpawnNextBomb = BOMB_SPAWN_TIME_START;

    _bombHouseTop = new BombHouse({0,200},0,1, BOMBHOUSE_TOP);
    _bombHouseBottom = new BombHouse({0,200},0,1, BOMBHOUSE_BOTTOM);

    didGameOver = false;
}

void GameManager::Update(float deltaTime)
{
    if (didGameOver) 
    {
        if (IsKeyPressed(KEY_R)) 
        {
            StartGame();
            return;
        }
    }

    timeToSpawnNextBomb -= deltaTime;
    if (timeToSpawnNextBomb <= 0 && !didGameOver) {
        bombSpawnTime = max(bombSpawnTime - 0.25f, 1.f);
        timeToSpawnNextBomb = bombSpawnTime;

        int spawnPos = rand() % 2;
        if (spawnPos == 1) {
            InstantiateBomb(new Bomb({-250,0}, 0, 150));
        } else {
            InstantiateBomb(new Bomb({250,0}, 0, 150));
        }
    }

    for (GameObject* o : _bombGameObjects) o->Update(deltaTime);

    _bombHouseTop->Update(deltaTime);
    _bombHouseBottom->Update(deltaTime);
}

void GameManager::Render(const float deltaTime) 
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    _cam.target = { 0, 0 };
    _cam.offset = { width / 2.0f, height / 2.0f }; // Screen center
    _cam.rotation = 0.0f;
    _cam.zoom = (float)height / MAP_COORD_SIZE;

    BeginDrawing();
    BeginMode2D(_cam);
    ClearBackground(RAYWHITE);

    DrawTexturePro(
        _sprMapBG,
        { 0, 0, MAP_SPRTE_SIZE, MAP_SPRTE_SIZE },  // SOURCE
        { -MAP_COORD_RADIUS, -MAP_COORD_RADIUS, MAP_COORD_SIZE, MAP_COORD_SIZE }, // DEST
        { 0, 0 }, // ORIGIN
        0.0f,
        WHITE
    );

    for (GameObject* o : _bombGameObjects) o->Render(deltaTime);

    _bombHouseTop->Render(deltaTime);
    _bombHouseBottom->Render(deltaTime);

    if (didGameOver) DrawText("Has mort :)", 190, 100, 20, RED);

    DrawRectangleLines(-250, -500, 500, 1000, GREEN);
    DrawRectangleLines(MAP_COORD_HOR_MIN, MAP_COORD_VER_MIN, (MAP_COORD_HOR_MAX - MAP_COORD_HOR_MIN), (MAP_COORD_VER_MAX - MAP_COORD_VER_MIN), WHITE);

    EndDrawing();
}

void GameManager::GameOver()
{
    didGameOver = true;

    for (GameObject* o : _bombGameObjects) o->GameOver();

    _bombHouseTop->GameOver();
    _bombHouseBottom->GameOver();
}

void GameManager::InstantiateBomb(Bomb* obj)
{
    _bombGameObjects.push_back(obj);
}

void GameManager::DestroyBomb(Bomb* obj)
{
    
}

int GameManager::GetBombReleasedState(Bomb* obj)
{
    BombType type = obj->GetType();

    if (_bombHouseTop->GetIsMouseInside(obj)) {
        //return (type == BOMB_LEFT) ? 1 : -1;
    }
    if (_bombHouseBottom->GetIsMouseInside(obj)) {
        //return (type == BOMB_RIGHT) ? 2 : -1;
    }

    return 3;
}

Vector2 GameManager::GetWorldMousePos() const
{
    return GetScreenToWorld2D(GetMousePosition(), _cam);
}