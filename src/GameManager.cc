#include "GameManager.hh"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager()
{
    GameManager::instance = this;

    sprBombBody = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMB_BODY).c_str());
    sprBombDeco = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMB_DECO).c_str());
    sprBombFuse = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMB_FUSE).c_str());
    sprBombHouse = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMBHOUSE).c_str());
    sprBombHouseScreen = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_BOMBHOUSE_SCREEN).c_str());
    sprExplosion = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_EXPLOSION).c_str());
    sprExplosionProps = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_EXPLOSION_PROPS).c_str());
    sprGameOverOverlay = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_GAMEOVER_OVERLAY).c_str());
    _sprMapBG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_BG).c_str());
    _sprMapMG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_MG).c_str());
    _sprMapFG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_FG).c_str());
    _sprSmallScreen = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_SMALL_SCREEN).c_str());
    _sprScreenNumbers = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_SCREEN_NUMBERS).c_str());

	audioManager = std::make_unique<AudioManager>();

    _bombHouseTop = std::make_unique<BombHouse>(Vector2{ 0,-425 }, 0, 256, BOMBHOUSE_TOP);
    _bombHouseBottom = std::make_unique<BombHouse>(Vector2{ 0,425 }, 0, 256, BOMBHOUSE_BOTTOM);

    _gameOverOverlay = std::make_unique<GameOverOverlay>(Vector2{ 0,0 }, 0, 1000);

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _playerExited = false;

    _highScore = 10;
    _pointTallyCounter = 0;
    _pointTallyCounterLast = 0;

    StartMainMenu();
}

GameManager::~GameManager()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _bombHouseTop.reset();
    _bombHouseBottom.reset();
    _gameOverOverlay.reset();

    GameManager::instance = nullptr;

    UnloadTexture(sprBombBody);
	UnloadTexture(sprBombDeco);
	UnloadTexture(sprBombFuse);
	UnloadTexture(sprBombHouse);
	UnloadTexture(sprBombHouseScreen);
	UnloadTexture(sprExplosion);
	UnloadTexture(sprExplosionProps);
	UnloadTexture(sprGameOverOverlay);
	UnloadTexture(_sprMapBG);
	UnloadTexture(_sprMapMG);
	UnloadTexture(_sprMapFG);
    UnloadTexture(_sprSmallScreen);
	UnloadTexture(_sprScreenNumbers);
}

void GameManager::StartMainMenu()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _roundValues.score = 0;
    _roundValues.spawnableBombTypes = { BOMB_MENU };
    for (int i = 0; i < BOMB_TYPE_COUNT; ++i) _roundValues.spawnedBombTypes[i] = 0;
    _roundValues.currentBombHouseTopType = BOMB_MENU;
    _roundValues.currentBombHouseBottomType = BOMB_MENU;
    _roundValues.roundTimeElapsed = 0;
    _roundValues.timeToSpawnNextBomb = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.nextBombSpawnTime = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.currentMaxBombs = 1;
    _roundValues.timeToChangeBombHouse = ROUND_BOMB_HOUSE_CHANGE_TIME_START;
    _roundValues.nextBombHouseChangeTime = ROUND_BOMB_HOUSE_CHANGE_TIME_START;
    _roundValues.timeForNextDramaticDrum = ROUND_TIME_FOR_DRAMATIC_DRUM;
    _roundValues.addedBlue = false;
    _roundValues.addedGreen = false;
    _roundValues.canRevive = false;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _state = MAIN_MENU;
}

void GameManager::StartGame()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _roundValues.score = 0;
    _roundValues.spawnableBombTypes = { BOMB_BLACK, BOMB_RED };
    for (int i = 0; i < BOMB_TYPE_COUNT; ++i) _roundValues.spawnedBombTypes[i] = 0;
    _roundValues.currentBombHouseTopType = (GetRandomValue(0, 1) == 0) ? BOMB_BLACK : BOMB_RED;
    _roundValues.currentBombHouseBottomType = (_roundValues.currentBombHouseTopType == BOMB_BLACK) ? BOMB_RED : BOMB_BLACK;
    _roundValues.roundTimeElapsed = 0;
    _roundValues.timeToSpawnNextBomb = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.nextBombSpawnTime = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.currentMaxBombs = ROUND_MAX_BOMBS_START;
    _roundValues.timeToChangeBombHouse = ROUND_BOMB_HOUSE_CHANGE_TIME_START;
    _roundValues.nextBombHouseChangeTime = ROUND_BOMB_HOUSE_CHANGE_TIME_START;
    _roundValues.addedBlue = false;
    _roundValues.addedGreen = false;
    _roundValues.timeForNextDramaticDrum = ROUND_TIME_FOR_DRAMATIC_DRUM;
    _roundValues.canRevive = true;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

	audioManager->PlayMusic();

    _state = ROUND;
}

void GameManager::StartReviveDecision()
{
    _bombGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _roundValues.spawnableBombTypes = { BOMB_REVIVE };
    for (int i = 0; i < BOMB_TYPE_COUNT; ++i) _roundValues.spawnedBombTypes[i] = 0;
    _roundValues.currentBombHouseTopType = BOMB_REVIVE;
    _roundValues.currentBombHouseBottomType = BOMB_REVIVE;
    _roundValues.canRevive = false;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _state = REVIVE_DECISION;

    InstantiateBomb(std::make_unique<Bomb>(Vector2{ -GetBombSpawnPos(), MAP_COORD_VER_MIN}, 0, 150, BOMB_REVIVE));
}

void GameManager::StartPointTally()
{
    _bombGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _roundValues.currentBombHouseTopType = BOMB_POINT_TALLY;
    _roundValues.currentBombHouseBottomType = BOMB_POINT_TALLY;
    
    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _pointTallyCounter = 0;
    _pointTallyCounterLast = 0;
    _state = POINT_TALLY;
}

void GameManager::UpdateMainMenu(const float deltaTime)
{
    if (_roundValues.spawnedBombTypes[BOMB_MENU] == 0) InstantiateBomb(std::make_unique<Bomb>(Vector2{ -GetBombSpawnPos(), MAP_COORD_VER_MIN }, 0, 150, BOMB_MENU));
    
    HandleBombGrab();
}

void GameManager::UpdateRound(const float deltaTime)
{
    _roundValues.roundTimeElapsed += deltaTime;
    if (!_roundValues.addedBlue && _roundValues.roundTimeElapsed >= ROUND_BLUE_BOMB_SPAWN_TIME_START) // SPAWN BLUE BOMBS
    {
        _roundValues.spawnableBombTypes.push_back(BOMB_BLUE);
        _roundValues.addedBlue = true;
    }
    if (!_roundValues.addedGreen && _roundValues.roundTimeElapsed >= ROUND_GREEN_BOMB_SPAWN_TIME_START) // SPAWN GREEN BOMBS
    {
        _roundValues.spawnableBombTypes.push_back(BOMB_GREEN);
        _roundValues.addedGreen = true;
    }

    _roundValues.timeToChangeBombHouse -= deltaTime;
    if (_roundValues.timeToChangeBombHouse <= 0) // CHANGE HOUSE TYPES
    {
        _roundValues.nextBombHouseChangeTime = max(_roundValues.nextBombHouseChangeTime - ROUND_BOMB_HOUSE_CHANGE_TIME_INCREMENT, ROUND_BOMB_HOUSE_CHANGE_TIME_MIN);
        _roundValues.timeToChangeBombHouse = _roundValues.nextBombHouseChangeTime;
        ChangeBombHouseTypes();
    }

	if (_roundValues.timeForNextDramaticDrum > 0) _roundValues.timeForNextDramaticDrum -= deltaTime;

    _roundValues.currentMaxBombs += ROUND_MAX_BOMBS_INCREMENT * deltaTime;
    _roundValues.timeToSpawnNextBomb -= deltaTime;
    if (_roundValues.timeToSpawnNextBomb <= 0) // SPAWN BOMB
    {
        _roundValues.nextBombSpawnTime = max(_roundValues.nextBombSpawnTime - ROUND_BOMB_SPAWN_TIME_INCREMENT, ROUND_BOMB_SPAWN_TIME_MIN);
        _roundValues.timeToSpawnNextBomb = _roundValues.nextBombSpawnTime;

        if (_bombGameObjects.size() < (unsigned int)_roundValues.currentMaxBombs)
        {
            float verticalPos = (float)GetRandomValue(MAP_COORD_VER_MIN, MAP_COORD_VER_MAX);
            int spawnPos = GetRandomValue(0, 1);

            if (spawnPos == 1) InstantiateBomb(std::make_unique<Bomb>(Vector2{ -GetBombSpawnPos(), verticalPos }, 0, 150, GetNewBombType()));
            else InstantiateBomb(std::make_unique<Bomb>(Vector2{ GetBombSpawnPos(), verticalPos }, 0, 150, GetNewBombType()));
        }
    }

    HandleBombGrab();
}

void GameManager::UpdateGameOver(const float deltaTime)
{
    _gameOverOverlay->Update(deltaTime);

    _gameOverRestartTimer -= deltaTime;
    if (_gameOverRestartTimer <= 0 && _bombGameObjects.empty() && _explosionGameObjects.empty())
    {
        if (_roundValues.canRevive) StartReviveDecision();
        else StartMainMenu();
    }
}

void GameManager::UpdateGameOverCutscene(const float deltaTime)
{
    _gameOverOverlay->Update(deltaTime);

    _gameOverCutsceneTimer -= deltaTime;

    if (_gameOverCutsceneTimer <= 0) 
    {
        if (_gameOverBomb != nullptr && !_gameOverBomb->isMarkedForDestroy())
        {
            InstantiateExplosion(_gameOverBomb->GetPosition());
            DestroyBomb(_gameOverBomb);
            _gameOverBomb = nullptr;
        }

        audioManager->PlayGameOverJingleSound();

        _state = GAME_OVER;
    }
}

void GameManager::UpdateReviveDecision(const float deltaTime)
{
    if (_roundValues.spawnedBombTypes[BOMB_REVIVE] == 0 && _explosionGameObjects.empty()) StartPointTally();
    
    HandleBombGrab();
}

void GameManager::UpdatePointTally(const float deltaTime)
{
	_pointTallyCounter += POINT_TALLY_INCREMENT_SPEED * deltaTime;

	if (_pointTallyCounterLast != (int)_pointTallyCounter)
	{
		_pointTallyCounterLast = (int)_pointTallyCounter;
		audioManager->PlayPointSound();
	}

	if (_pointTallyCounter >= _roundValues.score)
	{
		if (_roundValues.score > _highScore)
		{
			_highScore = _roundValues.score;
			audioManager->PlayPointTallyEndHighScoreSound();
		}
		else
		{
			audioManager->PlayPointTallyEndSound();
		}

		_pointTallyCounter = (float)_roundValues.score;
		_pointTallyDoneTimer = 0;
		_state = POINT_TALLY_DONE;
	}
}

void GameManager::UpdatePointTallyDone(const float deltaTime)
{
	_pointTallyDoneTimer += deltaTime;
    if (_pointTallyDoneTimer >= POINT_TALLY_DONE_TIME_MAX) StartMainMenu();
	if (_pointTallyDoneTimer >= POINT_TALLY_DONE_TIME_MIN && IsMouseButtonPressed(0)) StartMainMenu();
}

void GameManager::Update(float deltaTime)
{
    audioManager->Update(deltaTime);

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
        case GAME_OVER_CUTSCENE:
        {
            UpdateGameOverCutscene(deltaTime);
            return; // Objects don't update during the cutscene, time effectively stops
            break;
        }
        case GAME_OVER:
        {
            UpdateGameOver(deltaTime);
            break;
        }
        case REVIVE_DECISION:
        {
            UpdateReviveDecision(deltaTime);
            break;
        }
        case POINT_TALLY:
        {
            UpdatePointTally(deltaTime);
            break;
        }
		case POINT_TALLY_DONE:
		{
			UpdatePointTallyDone(deltaTime);
			break;
		}
    }

    for (auto& o : _bombGameObjects) o->Update(deltaTime);
    for (auto& o : _explosionGameObjects) o->Update(deltaTime);

    CheckBombCollisions();

    _bombHouseTop->Update(deltaTime);
    _bombHouseBottom->Update(deltaTime);

    _screenShakeTrauma = fmaxf(0.0f, _screenShakeTrauma - SCREEN_SHAKE_DECAY_RATE * deltaTime);
    float shakeIntensity = _screenShakeTrauma * _screenShakeTrauma;

    _screenShakePhase += SCREEN_SHAKE_MAX_FREQUENCY * shakeIntensity * deltaTime;
    _screenShakePhase = fmodf(_screenShakePhase, 2.0f * PI);

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

    float shakeIntensity = _screenShakeTrauma * _screenShakeTrauma;

    _cam.target = { 0, 0 };
    _cam.offset = { width / 2.0f, height / 2.0f }; // Screen center
    _cam.rotation = sinf(_screenShakePhase) * SCREEN_SHAKE_MAX_ROTATION_DEGREES * shakeIntensity;
    _cam.zoom = (float)height / MAP_COORD_SIZE;

    BeginDrawing();
    BeginMode2D(_cam);
    ClearBackground(BLACK);

    // Map bg
    DrawTexturePro(
        _sprMapBG, { 0, 0, MAP_BG_SPRITE_SIZE_X, MAP_BG_SPRITE_SIZE_Y },
        { -MAP_BG_COORD_RADIUS_X, -MAP_BG_COORD_RADIUS_Y, MAP_BG_COORD_SIZE_X, MAP_BG_COORD_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    _bombHouseTop->Render(deltaTime);
    _bombHouseBottom->Render(deltaTime);

    // Map mg
    DrawTexturePro(
        _sprMapMG, { 0, 0, MAP_BG_SPRITE_SIZE_X, MAP_BG_SPRITE_SIZE_Y },
        { -MAP_BG_COORD_RADIUS_X, -MAP_BG_COORD_RADIUS_Y, MAP_BG_COORD_SIZE_X, MAP_BG_COORD_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    sort(_bombGameObjects.begin(), _bombGameObjects.end(),
         [](const std::unique_ptr<Bomb>& a, const std::unique_ptr<Bomb>& b) {
            return Bomb::BombLayerSort(a.get(), b.get());
         });


    for (auto& o : _bombGameObjects) o->Render(deltaTime);
    for (auto& o : _explosionGameObjects) o->Render(deltaTime);

    if (DEBUG_DRAW_MAP_BOUNDS)
    {
		Vector2 boundsH = GetHorizontalBounds();

        DrawRectangleLines(boundsH.x, -500, (boundsH.y - boundsH.x), 1000, GREEN);
        DrawLine(boundsH.x, BOMBHOUSE_COORD_BOT_VER_POS, boundsH.y, BOMBHOUSE_COORD_BOT_VER_POS, BLUE);
        DrawLine(boundsH.x, BOMBHOUSE_COORD_TOP_VER_POS, boundsH.y, BOMBHOUSE_COORD_TOP_VER_POS, BLUE);

        DrawRectangleLines(boundsH.x, MAP_COORD_VER_MIN, (boundsH.y - boundsH.x), (MAP_COORD_VER_MAX - MAP_COORD_VER_MIN), WHITE);
    }

    // Map fg
    DrawTexturePro(
        _sprMapFG, { 0, 0, MAP_BG_SPRITE_SIZE_X, MAP_BG_SPRITE_SIZE_Y },
        { -MAP_BG_COORD_RADIUS_X, -MAP_BG_COORD_RADIUS_Y, MAP_BG_COORD_SIZE_X, MAP_BG_COORD_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    // Screens
    _bombHouseTop->RenderScreen();
    _bombHouseBottom->RenderScreen();

    if (_grabbedBomb != nullptr && !_grabbedBomb->isMarkedForDestroy()) _grabbedBomb->Render(deltaTime); // Render grabbed bomb on top of everything


    DrawTexturePro(
        _sprSmallScreen, { 0, 0, SMALL_SCREEN_SPRITE_WIDTH, SMALL_SCREEN_SPRITE_HEIGHT },
        { GetSmallScreenPos(), SMALL_SCREEN_POSITION_Y, SMALL_SCREEN_SPRITE_WIDTH, SMALL_SCREEN_SPRITE_HEIGHT},
        { 0, 0 }, 0.0f, WHITE
    );

    int numberToDraw = _roundValues.score;
	Color numberColor = WHITE;
	if (_state == MAIN_MENU) { numberToDraw = _highScore; numberColor = SMALL_NUMBER_YELLOW_COLOR; }
	if ((_state == ROUND || _state == REVIVE_DECISION) && _roundValues.score > _highScore) numberColor = SMALL_NUMBER_YELLOW_COLOR;
    if (_state == GAME_OVER_CUTSCENE || _state == GAME_OVER || _state == POINT_TALLY || _state == POINT_TALLY_DONE) numberToDraw = -1; // -1 to show DEAD
    DrawScreenNumber(numberToDraw, 
        { GetSmallScreenPos() + SMALL_SCREEN_NUMBER_POSITION_OFFSET_X,
        SMALL_SCREEN_POSITION_Y + SMALL_SCREEN_NUMBER_POSITION_OFFSET_Y }, numberColor);

	if (_state == POINT_TALLY || _state == POINT_TALLY_DONE)
	{
		DrawScreenNumber((int)_pointTallyCounter, { BOMBHOUSE_SCREEN_TOP_COORD_X + BOMBHOUSE_SCREEN_TEXT_OFFSET_X, BOMBHOUSE_SCREEN_TOP_COORD_Y }, WHITE);
		DrawScreenNumber((int)_highScore, { BOMBHOUSE_SCREEN_BOT_COORD_X + BOMBHOUSE_SCREEN_TEXT_OFFSET_X, BOMBHOUSE_SCREEN_BOT_COORD_Y }, SMALL_NUMBER_YELLOW_COLOR);
	}

	// Gradient and black rectangles on map limits to hide the edges of the map
	DrawRectangleGradientH(-MAP_BG_COORD_RADIUS_X, -MAP_COORD_RADIUS, 60, MAP_COORD_SIZE, BLACK, { 0, 0, 0, 0 });
	DrawRectangleGradientH(MAP_BG_COORD_RADIUS_X - 60, -MAP_COORD_RADIUS, 60, MAP_COORD_SIZE, { 0, 0, 0, 0 }, BLACK);

    Vector2 boundsH = GetHorizontalBounds();
    const float maskExtent = MAP_BG_COORD_SIZE_X;
    const float maskY = -MAP_BG_COORD_RADIUS_Y;
    const float maskHeight = MAP_BG_COORD_SIZE_Y;
    DrawRectangleRec({ -maskExtent, maskY, maskExtent + boundsH.x, maskHeight }, BLACK);
    DrawRectangleRec({ boundsH.y,   maskY, maskExtent - boundsH.y, maskHeight }, BLACK);

    _gameOverOverlay->Render(deltaTime);

    DrawFPS(-500, -500);

    EndMode2D();
    EndDrawing();
}

void GameManager::GameOver(Bomb* obj)
{
    if (_state == GAME_OVER || _state == GAME_OVER_CUTSCENE) return;
    _state = GAME_OVER_CUTSCENE;
    _gameOverCutsceneTimer = GAMEOVER_CUTSCENE_TIME;
	_gameOverRestartTimer = GAMEOVER_RESTART_TIME;

    audioManager->StopMusic();
	audioManager->PlayGameOverAlertSound();

    _gameOverBomb = obj;

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

    _gameOverOverlay->SetPosition(obj->GetPosition());
    _gameOverOverlay->GameOver();
}

bool GameManager::IsGameOverCutscene() const
{
    return _state == GAME_OVER_CUTSCENE;
}

void GameManager::InstantiateBomb(std::unique_ptr<Bomb> obj)
{
    _roundValues.spawnedBombTypes[obj.get()->GetType()]++;
    _bombGameObjects.push_back(std::move(obj));
}

void GameManager::DestroyBomb(Bomb* obj)
{
    auto it = std::find_if(_bombGameObjects.begin(), _bombGameObjects.end(),
        [obj](const std::unique_ptr<Bomb>& p) { return p.get() == obj; });

    if (it != _bombGameObjects.end())
    {
        _roundValues.spawnedBombTypes[(*it)->GetType()]--;
        (*it)->MarkForDestroy();
    }
}

void GameManager::InstantiateExplosion(const Vector2 position)
{
    _explosionGameObjects.push_back(std::make_unique<Explosion>(position, 0, EXPLOSION_SIZE));

    AddScreenShake(EXPLOSION_SCREEN_SHAKE_TIME);
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

void GameManager::AddScreenShake(float amount)
{
	_screenShakeTrauma = fminf(_screenShakeTrauma + amount, 1.0f);
}

void GameManager::DrawScreenNumber(const int score, const Vector2 position, const Color color) const
{
    if (score > 9999) return;

    int digits[4] = { 0 };
    digits[0] = score / 1000;
    digits[1] = (score / 100) % 10;
    digits[2] = (score / 10) % 10;
    digits[3] = score % 10;

    if (score == -1) // Show DEAD
    {
        digits[0] = 10;
        digits[1] = 11;
        digits[2] = 12;
        digits[3] = 10;
    }

    for (int i = 0; i < 4; ++i)
    {
        DrawTexturePro(
            _sprScreenNumbers, { (float)(digits[i] * SCREEN_NUMBER_SPRITE_WIDTH), 0, SCREEN_NUMBER_SPRITE_WIDTH, SCREEN_NUMBER_SPRITE_HEIGHT },
            { position.x + (float)(i * (SCREEN_NUMBER_SPRITE_WIDTH + 8)), position.y, SCREEN_NUMBER_SPRITE_WIDTH, SCREEN_NUMBER_SPRITE_HEIGHT },
            { 0, 0 }, 0.0f, color
        );
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
    if (_state == MAIN_MENU) 
    {
        if (_placedDirection == BOMB_PLACED_TOP) StartGame();
        else _playerExited = true;
        return;
	}
    else if (_state == REVIVE_DECISION)
    {
        if (_placedDirection == BOMB_PLACED_TOP) int a = 0; // TODO
        else StartPointTally();
        return;
    }

    BombType type = obj->GetType();

    if (_placedDirection == BOMB_PLACED_TOP && !_bombHouseTop->GetIsBombEnteredTypeValid(type))
    {
        ExplodeBomb(obj);
    }
    else if (_placedDirection == BOMB_PLACED_BOT && !_bombHouseBottom->GetIsBombEnteredTypeValid(type))
    {
        ExplodeBomb(obj);
    }
    else 
    {
        if (_state == ROUND) 
        {
            _roundValues.score++;
            audioManager->PlayPointSound();
        }
        DestroyBomb(obj);
    }
}

void GameManager::ExplodeBomb(Bomb* obj)
{
    if (_state == ROUND)
    {
        GameOver(obj);
    }
    else
    {
        InstantiateExplosion(obj->GetPosition());
    	DestroyBomb(obj);
    }
}

void GameManager::HandleBombGrab()
{
    _currentPressed = IsMouseButtonDown(0);
    if (_grabbedBomb == nullptr && _currentPressed && !_prevPressed)
    {
        TryGrabBomb(GetWorldMousePos());
        if (_grabbedBomb != nullptr) 
        {
            audioManager->PlayBombGrabbedSound(GameManager::instance->GetPan(_grabbedBomb->GetPosition()));
            _grabbedBomb->Grab();
        }
    }
    else if (_grabbedBomb != nullptr && IsMouseButtonReleased(0))
    {
        if (!_grabbedBomb->isMarkedForDestroy()) 
        {
            int letGoState = _grabbedBomb->LetGo(GetBombReleasedState(_grabbedBomb));

            if (letGoState == BOMB_PLACED_TOP || letGoState == BOMB_PLACED_BOT) audioManager->PlayBombReleasedBombHouseSound(GameManager::instance->GetPan(_grabbedBomb->GetPosition()));
            else audioManager->PlayBombReleasedMetalSound(GameManager::instance->GetPan(_grabbedBomb->GetPosition()));

            if ( (letGoState == BOMB_PLACED_TOP && !_bombHouseTop->GetIsBombEnteredTypeValid(_grabbedBomb->GetType())) ||
                (letGoState == BOMB_PLACED_BOT && !_bombHouseBottom->GetIsBombEnteredTypeValid(_grabbedBomb->GetType())))
            {
                if (_roundValues.timeForNextDramaticDrum <= 0)
                {
                    _grabbedBomb->MoveToBombHouseMin();
                    audioManager->PlayDramaticDrum();
                    _roundValues.timeForNextDramaticDrum = ROUND_TIME_FOR_DRAMATIC_DRUM;
                    AddScreenShake(DRAMATIC_DRUM_SCREEN_SHAKE_TIME);
                }
            }
        }
        _grabbedBomb = nullptr;
    }

    _prevPressed = _currentPressed;
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

Vector2 GameManager::GetHorizontalBounds() const
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    float halfWidth = (width / 2.0f) / ((float)height / MAP_COORD_SIZE);
    halfWidth = min(halfWidth, (float)MAP_COORD_HOR_MAX);
    return Vector2{ -halfWidth, halfWidth };
}

float GameManager::GetBombSpawnPos() const
{
    return GetHorizontalBounds().y + (float)BOMB_SPAWN_POS_OFFSET;
}

float GameManager::GetSmallScreenPos() const
{
	return GetHorizontalBounds().y + (float)SMALL_SCREEN_POSITION_X_OFFSET;
}

float GameManager::GetPan(const Vector2& position) const
{
    Vector2 bounds = GetHorizontalBounds();
    float pan = (position.x - bounds.x) / (bounds.y - bounds.x);
    return 1.0f - pan;
}

BombType GameManager::GetNewBombType() const
{
    int chance = GetRandomValue(1, 100);
    if (chance < BOMB_CHANCE_TO_SPAWN_RANDOM_COLOR && _state == ROUND) // Random color from spawnable types
    {
        int index = GetRandomValue(0, _roundValues.spawnableBombTypes.size() - 1);
        return _roundValues.spawnableBombTypes[index];
    }
    else // The color with the least bombs
    {
        BombType leastType = BOMB_INVALID;
        for (int i = 0; i < (int)_roundValues.spawnableBombTypes.size(); ++i)
        {
            BombType t = _roundValues.spawnableBombTypes[i];
            if (leastType == BOMB_INVALID || _roundValues.spawnedBombTypes[t] < _roundValues.spawnedBombTypes[leastType])
            {
                if (t != BOMB_MENU) leastType = t;
            }
        }

        return leastType;
    }
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

bool GameManager::PlayerExited() const
{
    return _playerExited;
}