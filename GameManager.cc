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

    _timeToSpawnNextBomb = BOMB_SPAWN_TIME_START;

    _bombHouseTop = new BombHouse({0,200},0,1, BOMBHOUSE_TOP);
    _bombHouseBottom = new BombHouse({0,200},0,1, BOMBHOUSE_BOTTOM);

    _didGameOver = false;
}

void GameManager::Update(float deltaTime)
{
    if (_didGameOver) 
    {
        if (IsKeyPressed(KEY_R)) 
        {
            StartGame();
            return;
        }
    }

    _timeToSpawnNextBomb -= deltaTime;
    if (_timeToSpawnNextBomb <= 0 && !_didGameOver) 
    {
        _bombSpawnTime = max(_bombSpawnTime - 0.25f, 1.f);
        _timeToSpawnNextBomb = _bombSpawnTime;

        float verticalPos = (float)GetRandomValue(MAP_COORD_VER_MIN, MAP_COORD_VER_MAX);
        int spawnPos = GetRandomValue(0, 1);

        if (spawnPos == 1) InstantiateBomb(new Bomb({ BOMB_SPAWN_POS_X_LEFT, verticalPos }, 0, 150));
        else InstantiateBomb(new Bomb({ BOMB_SPAWN_POS_X_RIGHT, verticalPos }, 0, 150));
    }

    for (Bomb* o : _bombGameObjects) o->Update(deltaTime);

    unsigned int size = _bombGameObjects.size();
    for (unsigned int i = 0; i < size; ++i) {
        for (unsigned int j = i + 1; j < size; ++j) {
            _bombGameObjects[i]->CheckCollisionWith(*_bombGameObjects[j]);
        }
    }

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

    sort(_bombGameObjects.begin(), _bombGameObjects.end(), Bomb::BombLayerSort);

    for (GameObject* o : _bombGameObjects) o->Render(deltaTime);

    _bombHouseTop->Render(deltaTime);
    _bombHouseBottom->Render(deltaTime);

    if (_didGameOver) DrawText("Has mort :)", 190, 100, 20, RED);

    DrawRectangleLines(-250, -500, 500, 1000, GREEN);
    DrawRectangleLines(MAP_COORD_HOR_MIN, MAP_COORD_VER_MIN, (MAP_COORD_HOR_MAX - MAP_COORD_HOR_MIN), (MAP_COORD_VER_MAX - MAP_COORD_VER_MIN), WHITE);
    DrawLine(MAP_COORD_HOR_MIN, BOMBHOUSE_COORD_BOT_VER_POS, MAP_COORD_HOR_MAX, BOMBHOUSE_COORD_BOT_VER_POS, BLUE);
    DrawLine(MAP_COORD_HOR_MIN, BOMBHOUSE_COORD_TOP_VER_POS, MAP_COORD_HOR_MAX, BOMBHOUSE_COORD_TOP_VER_POS, BLUE);

    DrawFPS(-500, -500);

    EndMode2D();
    EndDrawing();
}

void GameManager::GameOver()
{
    if (_didGameOver) return;
    _didGameOver = true;

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
    auto it = std::find(_bombGameObjects.begin(), _bombGameObjects.end(), obj);

    if (it != _bombGameObjects.end())
    {
        delete* it;
        _bombGameObjects.erase(it);
    }
}


int GameManager::GetBombReleasedState(Bomb* obj)
{
    Vector2 mousePos = GetWorldMousePos();

    if (mousePos.y >= BOMBHOUSE_COORD_BOT_VER_POS) return BOMB_RELEASED_BOT;
    if (mousePos.y <= BOMBHOUSE_COORD_TOP_VER_POS) return BOMB_RELEASED_TOP;

    return BOMB_RELEASED_DEF;
}

void GameManager::BombEntered(Bomb* obj, int _placedDirection)
{
    BombType type = obj->GetType();
    if (_placedDirection == BOMB_PLACED_TOP)
    {
        if (_bombHouseTop->GetType() != type) GameOver();
    }
    else if (_placedDirection == BOMB_PLACED_BOT)
    {
        if (_bombHouseBottom->GetType() != type) GameOver();
    }

    DestroyBomb(obj);
}

Vector2 GameManager::GetWorldMousePos() const
{
    return GetScreenToWorld2D(GetMousePosition(), _cam);
}