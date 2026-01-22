#include "GameManager.hh"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
{
    GameManager::instance = this;

    sprBombBody = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMB_BODY).c_str());
    sprBombDeco = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMB_DECO).c_str());
    sprExplosion = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_EXPLOSION).c_str());
    _sprMapBG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_BG).c_str());

    _state = ROUND;

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

    _bombHouseTop = std::make_unique<BombHouse>(Vector2{100,-500},0,1, BOMBHOUSE_TOP);
    _bombHouseBottom = std::make_unique<BombHouse>(Vector2{100,400},0,1, BOMBHOUSE_BOTTOM);

    _grabbedBomb = nullptr;

    _roundValues.score = 0;
    _roundValues.spawnableBombTypes = { BOMB_BLACK, BOMB_RED };
    _roundValues.currentBombHouseTopType = (GetRandomValue(0, 1) == 0) ? BOMB_BLACK : BOMB_RED;
    _roundValues.currentBombHouseBottomType = (_roundValues.currentBombHouseTopType == BOMB_BLACK) ? BOMB_RED : BOMB_BLACK;
    _roundValues.roundTimeElapsed = 0;
    _roundValues.timeToSpawnNextBomb = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.nextBombSpawnTime = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.timeToChangeBombHouse = ROUND_BOMB_HOUSE_CHANGE_TIME_START;
    _roundValues.nextBombHouseChangeTime = ROUND_BOMB_HOUSE_CHANGE_TIME_START;
    _roundValues.addedBlue = false;
    _roundValues.addedGreen = false;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _state = ROUND;
}

void GameManager::UpdateMainMenu(const float deltaTime)
{
}

void GameManager::UpdateRound(const float deltaTime)
{
    _roundValues.roundTimeElapsed += deltaTime;
    if (!_roundValues.addedBlue && _roundValues.roundTimeElapsed >= ROUND_BLUE_BOMB_SPAWN_TIME_START)
    {
        std::cout << "Blue bombs can now spawn!" << std::endl;
        _roundValues.spawnableBombTypes.push_back(BOMB_BLUE);
        _roundValues.addedBlue = true;
    }
    if (!_roundValues.addedGreen && _roundValues.roundTimeElapsed >= ROUND_GREEN_BOMB_SPAWN_TIME_START)
    {
        std::cout << "Green bombs can now spawn!" << std::endl;
        _roundValues.spawnableBombTypes.push_back(BOMB_GREEN);
        _roundValues.addedGreen = true;
    }

    _roundValues.timeToChangeBombHouse -= deltaTime;
    if (_roundValues.timeToChangeBombHouse <= 0)
    {
        _roundValues.nextBombHouseChangeTime = max(_roundValues.nextBombHouseChangeTime - ROUND_BOMB_HOUSE_CHANGE_TIME_INCREMENT, ROUND_BOMB_HOUSE_CHANGE_TIME_MIN);
        _roundValues.timeToChangeBombHouse = _roundValues.nextBombHouseChangeTime;
        ChangeBombHouseTypes();
    }

    _roundValues.timeToSpawnNextBomb -= deltaTime;
    if (_roundValues.timeToSpawnNextBomb <= 0) 
    {
        _roundValues.nextBombSpawnTime = max(_roundValues.nextBombSpawnTime - ROUND_BOMB_SPAWN_TIME_INCREMENT, ROUND_BOMB_SPAWN_TIME_MIN);
        _roundValues.timeToSpawnNextBomb = _roundValues.nextBombSpawnTime;

        float verticalPos = (float)GetRandomValue(MAP_COORD_VER_MIN, MAP_COORD_VER_MAX);
        int spawnPos = GetRandomValue(0, 1);

        if (spawnPos == 1) InstantiateBomb(std::make_unique<Bomb>(Vector2{ BOMB_SPAWN_POS_X_LEFT, verticalPos }, 0, 150, GetNewBombType()));
        else InstantiateBomb(std::make_unique<Bomb>(Vector2{ BOMB_SPAWN_POS_X_RIGHT, verticalPos }, 0, 150, GetNewBombType()));
    }

    // GRAB BOMB
    if (_grabbedBomb == nullptr && IsMouseButtonPressed(0))
    {
        TryGrabBomb(GetWorldMousePos());
        if (_grabbedBomb != nullptr) _grabbedBomb->Grab();
    } 
	else if (_grabbedBomb != nullptr && IsMouseButtonReleased(0))
    {
        if (!_grabbedBomb->isMarkedForDestroy()) 
        {
            _grabbedBomb->LetGo(GetBombReleasedState(_grabbedBomb));
        }
        _grabbedBomb = nullptr;
	}
}

void GameManager::UpdateGameOver(const float deltaTime)
{
    if (IsKeyPressed(KEY_R))
    {
        StartGame();
        return;
    }
}

void GameManager::Update(float deltaTime)
{
    switch (_state)
    {
        case MAIN_MENU: 
        {
            UpdateMainMenu(deltaTime);
            break;
        }
        case ROUND:
        {
            UpdateRound(deltaTime);
            break;
        }
        case GAME_OVER:
        {
            UpdateGameOver(deltaTime);
            break;
        }
    }

    for (auto& o : _bombGameObjects) o->Update(deltaTime);
    for (auto& o : _explosionGameObjects) o->Update(deltaTime);

    CheckBombCollisions();

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

    DrawText(TextFormat("%d", _roundValues.score), -240, -490, 40, BLACK);
    if (_state == GAME_OVER) DrawText("Has mort :)", 0, 0, 40, RED);

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
    if (_state == GAME_OVER) return;
    _state = GAME_OVER;

    if (_grabbedBomb != nullptr)
    {
        if (!_grabbedBomb->isMarkedForDestroy()) 
        {
            _grabbedBomb->LetGo(GetBombReleasedState(_grabbedBomb));
        }
        _grabbedBomb = nullptr;
	}

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

    if (_placedDirection == BOMB_PLACED_TOP && !_bombHouseTop->GetIsBombEnteredTypeValid(type))
    {
        InstantiateExplosion(obj->GetPosition());
        GameOver();
    }
    else if (_placedDirection == BOMB_PLACED_BOT && !_bombHouseBottom->GetIsBombEnteredTypeValid(type))
    {
        InstantiateExplosion(obj->GetPosition());
        GameOver();
    }
    else 
    {
        if (_state == ROUND) _roundValues.score++;
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

void GameManager::CheckBombCollisions()
{
    unsigned int size = _bombGameObjects.size();
    for (unsigned int i = 0; i < size; ++i) 
    {
        for (unsigned int j = i + 1; j < size; ++j) 
        {
            _bombGameObjects[i]->CheckCollisionWith(*_bombGameObjects[j]);
        }
    }
}

Vector2 GameManager::GetWorldMousePos() const
{
    return GetScreenToWorld2D(GetMousePosition(), _cam);
}

BombType GameManager::GetNewBombType() const
{
    int index = GetRandomValue(0, _roundValues.spawnableBombTypes.size() - 1);
    return _roundValues.spawnableBombTypes[index];
}

void GameManager::ChangeBombHouseTypes()
{
    if (_roundValues.spawnableBombTypes.size() < 2) return; // Invalid case, do nothing
    if (_roundValues.spawnableBombTypes.size() == 2) // 2 colors: swap them
    {
        BombType t = _roundValues.currentBombHouseBottomType;
        _roundValues.currentBombHouseBottomType = _roundValues.currentBombHouseTopType;
        _roundValues.currentBombHouseTopType = t;
    }
    else if (_roundValues.spawnableBombTypes.size() == 3) // 3 colors: swap colors, and one house must have the missing color
    {
        BombType t = _roundValues.currentBombHouseBottomType;
        _roundValues.currentBombHouseBottomType = _roundValues.currentBombHouseTopType;
        _roundValues.currentBombHouseTopType = t;

        BombType missing = BOMB_BLACK;
        for (auto c : _roundValues.spawnableBombTypes)
        {
            if (c != _roundValues.currentBombHouseTopType && c != _roundValues.currentBombHouseBottomType) { missing = c; break; }
        }

        if (GetRandomValue(0,1) == 0) _roundValues.currentBombHouseTopType = missing;
        else _roundValues.currentBombHouseBottomType = missing;
    } 
    else // 4 or more: both houses must change and have a color not present before
    {
        BombType newTopType;
        BombType newBottomType;
        do 
        {
            int indexT = GetRandomValue(0, _roundValues.spawnableBombTypes.size() - 1);
            int indexB = GetRandomValue(0, _roundValues.spawnableBombTypes.size() - 1);
            newTopType = _roundValues.spawnableBombTypes[indexT];
            newBottomType = _roundValues.spawnableBombTypes[indexB];
        } 
        while (newTopType == _roundValues.currentBombHouseTopType ||
            newBottomType == _roundValues.currentBombHouseBottomType ||
            newBottomType == _roundValues.currentBombHouseTopType ||
            newTopType == _roundValues.currentBombHouseBottomType ||
            newTopType == newBottomType
        );
        _roundValues.currentBombHouseTopType = newTopType;
        _roundValues.currentBombHouseBottomType = newBottomType;        
    }

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, false);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, false);
}