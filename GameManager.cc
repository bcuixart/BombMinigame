#include "GameManager.hh"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
{
    GameManager::instance = this;

    sprBombBody = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMB_BODY).c_str());
    sprBombDeco = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMB_DECO).c_str());
    sprExplosion = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_EXPLOSION).c_str());
    _sprMapBG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_BG).c_str());

    StartGame();
}

GameManager::~GameManager()
{
    GameManager::instance = nullptr;

    UnloadTexture(sprBombBody);
	UnloadTexture(sprBombDeco);
	UnloadTexture(sprExplosion);
	UnloadTexture(_sprMapBG);
}

void GameManager::StartGame()
{
    // Clear vectors, unique_ptr automatically frees memory
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _timeToSpawnNextBomb = BOMB_SPAWN_TIME_START;

    _bombHouseTop = std::make_unique<BombHouse>(Vector2{0,200},0,1, BOMBHOUSE_TOP);
    _bombHouseBottom = std::make_unique<BombHouse>(Vector2{0,200},0,1, BOMBHOUSE_BOTTOM);

    _grabbedBomb = nullptr;

    _didGameOver = false;
}

void GameManager::Update(float deltaTime)
{
    if (_didGameOver && IsKeyPressed(KEY_R))
    {
        StartGame();
        return;
    }

    _timeToSpawnNextBomb -= deltaTime;
    if (_timeToSpawnNextBomb <= 0 && !_didGameOver) 
    {
        _bombSpawnTime = max(_bombSpawnTime - 0.25f, 1.f);
        _timeToSpawnNextBomb = _bombSpawnTime;

        float verticalPos = (float)GetRandomValue(MAP_COORD_VER_MIN, MAP_COORD_VER_MAX);
        int spawnPos = GetRandomValue(0, 1);

        if (spawnPos == 1) InstantiateBomb(std::make_unique<Bomb>(Vector2{ BOMB_SPAWN_POS_X_LEFT, verticalPos }, 0, 150));
        else InstantiateBomb(std::make_unique<Bomb>(Vector2{ BOMB_SPAWN_POS_X_RIGHT, verticalPos }, 0, 150));
    }

    for (auto& o : _bombGameObjects) o->Update(deltaTime);
    for (auto& o : _explosionGameObjects) o->Update(deltaTime);

    // GRAB BOMB
    unsigned int size = _bombGameObjects.size();
    Vector2 mousePos = GetWorldMousePos();
    if (!_didGameOver && _grabbedBomb == nullptr && IsMouseButtonPressed(0))
    {
        TryGrabBomb(mousePos);
        if (_grabbedBomb != nullptr) _grabbedBomb->Grab();
    } 
	else if (_grabbedBomb != nullptr && (IsMouseButtonReleased(0) || _didGameOver))
    {
        if (!_grabbedBomb->isMarkedForDestroy()) 
        {
            _grabbedBomb->LetGo(GetBombReleasedState(_grabbedBomb));
        }
        _grabbedBomb = nullptr;
	}

    // COLLISION CHECK
    for (unsigned int i = 0; i < size; ++i) 
    {
        for (unsigned int j = i + 1; j < size; ++j) 
        {
            _bombGameObjects[i]->CheckCollisionWith(*_bombGameObjects[j]);
        }
    }

    _bombHouseTop->Update(deltaTime);
    _bombHouseBottom->Update(deltaTime);

    _bombGameObjects.erase(std::remove_if(_bombGameObjects.begin(), _bombGameObjects.end(),
        [](const std::unique_ptr<Bomb>& p) { return p->isMarkedForDestroy(); }),
        _bombGameObjects.end());

    _explosionGameObjects.erase(std::remove_if(_explosionGameObjects.begin(), _explosionGameObjects.end(),
        [](const std::unique_ptr<Explosion>& p) { return p->isMarkedForDestroy(); }),
        _explosionGameObjects.end());

    if (_grabbedBomb != nullptr && _grabbedBomb->isMarkedForDestroy()) _grabbedBomb = nullptr;
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

    sort(_bombGameObjects.begin(), _bombGameObjects.end(),
         [](const std::unique_ptr<Bomb>& a, const std::unique_ptr<Bomb>& b) {
            return Bomb::BombLayerSort(a.get(), b.get());
         });

    for (auto& o : _bombGameObjects) o->Render(deltaTime);
    for (auto& o : _explosionGameObjects) o->Render(deltaTime);

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

    for (auto& o : _bombGameObjects) o->GameOver();

    _bombHouseTop->GameOver();
    _bombHouseBottom->GameOver();
}

void GameManager::InstantiateBomb(std::unique_ptr<Bomb> obj)
{
    _bombGameObjects.push_back(std::move(obj));
}

void GameManager::DestroyBomb(Bomb* obj)
{
    auto it = std::find_if(_bombGameObjects.begin(), _bombGameObjects.end(),
        [obj](const std::unique_ptr<Bomb>& p) { return p.get() == obj; });

    if (it != _bombGameObjects.end())
    {
        (*it)->MarkForDestroy();
    }
}

void GameManager::InstantiateExplosion(const Vector2 position)
{
    _explosionGameObjects.push_back(std::make_unique<Explosion>(position, 0, EXPLOSION_SIZE));
}

void GameManager::DestroyExplosion(Explosion* expl)
{
    auto it = std::find_if(_explosionGameObjects.begin(), _explosionGameObjects.end(),
        [expl](const std::unique_ptr<Explosion>& p) { return p.get() == expl; });

    if (it != _explosionGameObjects.end())
    {
        (*it)->MarkForDestroy();
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

    if (_placedDirection == BOMB_PLACED_TOP && _bombHouseTop->GetType() != type)
    {
        InstantiateExplosion(obj->GetPosition());
        GameOver();
    }
    else if (_placedDirection == BOMB_PLACED_BOT && _bombHouseBottom->GetType() != type)
    {
        InstantiateExplosion(obj->GetPosition());
        GameOver();
    }

    DestroyBomb(obj);
}

void GameManager::TryGrabBomb(const Vector2 mousePos)
{
    unsigned int size = _bombGameObjects.size();
    // FIND CLOSEST BOMB FROM GRABBED ONES
    for (unsigned int i = 0; i < size; ++i)
    {
        if (_bombGameObjects[i]->WasClicked(mousePos))
        {
            if (_grabbedBomb == nullptr) _grabbedBomb = _bombGameObjects[i].get();
            else
            {
                float distCurrent = Vector2Distance(mousePos, _bombGameObjects[i]->GetPosition());
                float distGrabbed = Vector2Distance(mousePos, _grabbedBomb->GetPosition());
                if (distCurrent < distGrabbed) _grabbedBomb = _bombGameObjects[i].get();
            }
        }
    }
}

Vector2 GameManager::GetWorldMousePos() const
{
    return GetScreenToWorld2D(GetMousePosition(), _cam);
}