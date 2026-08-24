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
    sprPipeValve = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_PIPE_VALVE).c_str());
    sprPipeValveIndicator = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_PIPE_VALVE_INDICATOR).c_str());
    sprSteamCloud = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_STEAM_CLOUD).c_str());
    sprSteamEmitter = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_STEAM_EMITTER).c_str());
    sprSteamIndicatorBase = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_STEAM_INDICATOR_BASE).c_str());
    sprSteamIndicatorNeedle = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_STEAM_INDICATOR_NEEDLE).c_str());
    _sprMapBG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_BG).c_str());
    _sprMapMG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_MG).c_str());
    _sprMapFG = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MAP_FG).c_str());
    _sprSmallScreen = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_SMALL_SCREEN).c_str());
    _sprScreenNumbers = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_SCREEN_NUMBERS).c_str());
	_sprMenuInfo = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_MENU_INFO).c_str());
    _sprPauseMenu = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_PAUSE_MENU).c_str());
    _sprPauseButton = LoadTexture((std::string(ASSETS_PATH) + ASSET_SPRITES_PATH + ASSET_SPRITE_PAUSE_BUTTON).c_str());

	audioManager = std::make_unique<AudioManager>();

    _bombHouseTop = std::make_unique<BombHouse>(Vector2{ 0,-640 }, 0, 256, BOMBHOUSE_TOP);
    _bombHouseBottom = std::make_unique<BombHouse>(Vector2{ 0,375 }, 0, 256, BOMBHOUSE_BOTTOM);

    _pipe = std::make_unique<Pipe>(Vector2 PIPE_POSITION, 0, 256);

    _gameOverOverlay = std::make_unique<GameOverOverlay>(Vector2{ 0,0 }, 0, 1000);

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _grabbedPipe = nullptr;

    _pauseButtonHovered = false;
	_pauseButtonHoveredLastFrame = false;

    _playerExited = false;

    _highScore = HIGHSCORE_DEFAULT;
    _pointTallyCounter = 0;
    _pointTallyCounterLast = 0;
    _pointTallySkipClicks = 0;

    _reviveHouseTopType = BOMB_INVALID;
    _reviveHouseBottomType = BOMB_INVALID;

	_pauseResumeTicks = 0;
	_pauseResumeTimer = 0.0f;

    _musicPrevTime = 0.0f;

	_menuInfoState = MENU_INFO_NONE;
    _menuInfoPage = 0;
	_pauseState = PAUSE_NONE;
    _pauseDecoVariation = 0;

    GetSaveData();

    StartMainMenu();
}

GameManager::~GameManager()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _bombHouseTop.reset();
    _bombHouseBottom.reset();
    _pipe.reset();
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
    UnloadTexture(sprPipeValve);
	UnloadTexture(sprPipeValveIndicator);
    UnloadTexture(sprSteamCloud);
    UnloadTexture(sprSteamEmitter);
    UnloadTexture(sprSteamIndicatorBase);
    UnloadTexture(sprSteamIndicatorNeedle);
	UnloadTexture(_sprMapBG);
	UnloadTexture(_sprMapMG);
	UnloadTexture(_sprMapFG);
    UnloadTexture(_sprSmallScreen);
	UnloadTexture(_sprScreenNumbers);
	UnloadTexture(_sprMenuInfo);
	UnloadTexture(_sprPauseMenu);
	UnloadTexture(_sprPauseButton);

    SaveData();
}

void GameManager::StartMainMenu()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _grabbedPipe = nullptr;

    _roundValues.score = 0;
    _roundValues.spawnableBombTypes = { BOMB_MENU };
    for (int i = 0; i < BOMB_TYPE_COUNT; ++i) _roundValues.spawnedBombTypes[i] = 0;
    _roundValues.currentBombHouseTopType = BOMB_MENU;
    _roundValues.currentBombHouseBottomType = BOMB_MENU;
    _roundValues.roundTimeElapsed = 0;
    _roundValues.timeToSpawnNextBomb = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.nextBombSpawnTime = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.currentMaxBombs = 1;
    _roundValues.timeForNextDramaticDrum = ROUND_TIME_FOR_DRAMATIC_DRUM;
    _roundValues.addedBlue = false;
    _roundValues.addedGreen = false;
    _roundValues.canRevive = false;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _state = MAIN_MENU;
    _menuInfoState = MENU_INFO_NONE;
    _pauseState = PAUSE_NONE;

    _pauseButtonHovered = false;
    _pauseButtonHoveredLastFrame = false;
}

void GameManager::StartMenuInfo()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _grabbedPipe = nullptr;

    _state = MENU_INFO;
    _menuInfoState = MENU_INFO_NONE;
    _menuInfoPage = 0;
    _pauseState = PAUSE_NONE;

    _pauseButtonHovered = false;
    _pauseButtonHoveredLastFrame = false;
}

void GameManager::StartGame()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _grabbedPipe = nullptr;

    _musicPrevTime = 0.0f;

    _roundValues.score = 0;
    _roundValues.spawnableBombTypes = { BOMB_BLACK, BOMB_RED };
    for (int i = 0; i < BOMB_TYPE_COUNT; ++i) _roundValues.spawnedBombTypes[i] = 0;
    _roundValues.currentBombHouseTopType = (GetRandomValue(0, 1) == 0) ? BOMB_BLACK : BOMB_RED;
    _roundValues.currentBombHouseBottomType = (_roundValues.currentBombHouseTopType == BOMB_BLACK) ? BOMB_RED : BOMB_BLACK;
    _roundValues.roundTimeElapsed = 0;
    _roundValues.timeToSpawnNextBomb = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.nextBombSpawnTime = ROUND_BOMB_SPAWN_TIME_START;
    _roundValues.currentMaxBombs = ROUND_MAX_BOMBS_START;
    _roundValues.musicChangeBombHouseIndex = 1; // To give more time before the first change
    _roundValues.addedBlue = false;
    _roundValues.addedGreen = false;
    _roundValues.timeForNextDramaticDrum = ROUND_TIME_FOR_DRAMATIC_DRUM;
    _roundValues.canRevive = true;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _pipe->StartGame();

	audioManager->PlayMusic();

    _state = ROUND;
    _menuInfoState = MENU_INFO_NONE;
    _pauseState = PAUSE_NONE;

    _pauseButtonHovered = false;
    _pauseButtonHoveredLastFrame = false;

    if (DEBUG_SPAWN_MAX_DEVICE_BOMBS_START)
    {
        for (int i = 0; i < GetDeviceMaxBombs(); ++i) SpawnBombRound();
    }
}

void GameManager::StartGameRevive()
{
    _bombGameObjects.clear();
    _explosionGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _grabbedPipe = nullptr;

    _musicPrevTime = 0.0f;

    for (int i = 0; i < BOMB_TYPE_COUNT; ++i) _roundValues.spawnedBombTypes[i] = 0;
    _roundValues.currentBombHouseTopType = _reviveHouseTopType;
    _roundValues.currentBombHouseBottomType = _reviveHouseBottomType;
    _roundValues.musicChangeBombHouseIndex = 0;
    _roundValues.timeForNextDramaticDrum = ROUND_TIME_FOR_DRAMATIC_DRUM;
    _roundValues.canRevive = false;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _pipe->StartGame();

	audioManager->PlayMusic();

    _state = ROUND; 
    _menuInfoState = MENU_INFO_NONE;
    _pauseState = PAUSE_NONE;

    _pauseButtonHovered = false;
    _pauseButtonHoveredLastFrame = false;
}

void GameManager::StartReviveDecision()
{
    _bombGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _grabbedPipe = nullptr;

    for (int i = 0; i < BOMB_TYPE_COUNT; ++i) _roundValues.spawnedBombTypes[i] = 0;
    _roundValues.currentBombHouseTopType = BOMB_REVIVE;
    _roundValues.currentBombHouseBottomType = BOMB_REVIVE;
    _roundValues.canRevive = false;

    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _state = REVIVE_DECISION;
    _menuInfoState = MENU_INFO_NONE;
    _pauseState = PAUSE_NONE;

    _pauseButtonHovered = false;
    _pauseButtonHoveredLastFrame = false;

    float verticalPos = (float)GetRandomValue(MAP_COORD_VER_MIN, MAP_COORD_VER_MAX);
    int spawnPos = GetRandomValue(0, 1);

    if (spawnPos == 1) InstantiateBomb(std::make_unique<Bomb>(Vector2{ -GetBombSpawnPos(), verticalPos }, 0, 150, BOMB_REVIVE));
    else InstantiateBomb(std::make_unique<Bomb>(Vector2{ GetBombSpawnPos(), verticalPos }, 0, 150, BOMB_REVIVE));
}

void GameManager::StartPointTally()
{
    _bombGameObjects.clear();

    _grabbedBomb = nullptr;
    _gameOverBomb = nullptr;

    _grabbedPipe = nullptr;

    _roundValues.currentBombHouseTopType = BOMB_POINT_TALLY;
    _roundValues.currentBombHouseBottomType = BOMB_POINT_TALLY;
    
    _bombHouseTop->SetType(_roundValues.currentBombHouseTopType, true);
    _bombHouseBottom->SetType(_roundValues.currentBombHouseBottomType, true);

    _pointTallyCounter = 0;
    _pointTallyCounterLast = 0;
	_pointTallySkipClicks = 0;
    _state = POINT_TALLY;
    _menuInfoState = MENU_INFO_NONE;
    _pauseState = PAUSE_NONE;

    _pauseButtonHovered = false;
    _pauseButtonHoveredLastFrame = false;
}

void GameManager::UpdateMainMenu(const float deltaTime)
{
    if (_roundValues.spawnedBombTypes[BOMB_MENU] == 0)
    {
        float verticalPos = (float)GetRandomValue(MAP_COORD_VER_MIN, MAP_COORD_VER_MAX);
        int spawnPos = GetRandomValue(0, 1);

        if (spawnPos == 1) InstantiateBomb(std::make_unique<Bomb>(Vector2{ -GetBombSpawnPos(), verticalPos }, 0, 150, BOMB_MENU));
        else InstantiateBomb(std::make_unique<Bomb>(Vector2{ GetBombSpawnPos(), verticalPos }, 0, 150, BOMB_MENU));
    }
    
    HandleBombGrab();
}

void GameManager::UpdateMenuInfo(const float deltaTime)
{
    Vector2 mousePos = GetWorldMousePos();

    const Rectangle nextButtonRect = { MENU_INFO_SPRITE_POSITION_X + MENU_INFO_BUTTON_NEXT_OFFSET_X, MENU_INFO_SPRITE_POSITION_Y + MENU_INFO_BUTTON_NEXT_OFFSET_Y, MENU_INFO_BUTTON_SIZE_X, MENU_INFO_BUTTON_SIZE_Y };
    const Rectangle exitButtonRect = { MENU_INFO_SPRITE_POSITION_X + MENU_INFO_BUTTON_EXIT_OFFSET_X, MENU_INFO_SPRITE_POSITION_Y + MENU_INFO_BUTTON_EXIT_OFFSET_Y, MENU_INFO_BUTTON_SIZE_X, MENU_INFO_BUTTON_SIZE_Y };

    if (CheckCollisionPointRec(mousePos, nextButtonRect))
    {
        if (_menuInfoState != MENU_INFO_NEXT) { _menuInfoState = MENU_INFO_NEXT; audioManager->PlayBombGrabbedSound(0); }
    }
    else if (CheckCollisionPointRec(mousePos, exitButtonRect))
    {
        if (_menuInfoState != MENU_INFO_EXIT) { _menuInfoState = MENU_INFO_EXIT; audioManager->PlayBombGrabbedSound(0); }
    }
    else
    {
        if (_menuInfoState != MENU_INFO_NONE) { _menuInfoState = MENU_INFO_NONE; audioManager->PlayBombGrabbedSound(0); }
    }

    if (_menuInfoState == MENU_INFO_NEXT && !_currentPressed && _prevPressed)
    {
        audioManager->PlayBombReleasedBombHouseSound(0);
		_menuInfoPage = (_menuInfoPage + 1) % MENU_INFO_PAGES;
    }

    if (_menuInfoState == MENU_INFO_EXIT && !_currentPressed && _prevPressed)
    {
        audioManager->PlayBombReleasedBombHouseSound(0);
        StartMainMenu();
    }
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

    // Bomb house music change logic
    float curTime = audioManager->GetMusicTime();
    float target = ROUND_BOMB_HOUSE_MUSIC_CHANGE_TIMES_ARRAY[_roundValues.musicChangeBombHouseIndex];

    if (curTime < _musicPrevTime - 1.0f) _roundValues.musicChangeBombHouseIndex = 1; // Loop
    else if (_musicPrevTime < target && curTime >= target)
    {
        _roundValues.musicChangeBombHouseIndex++;
        if (_roundValues.musicChangeBombHouseIndex >= ROUND_BOMB_HOUSE_MUSIC_CHANGE_TIMES) _roundValues.musicChangeBombHouseIndex = 1;
        ChangeBombHouseTypes();
    }

    _musicPrevTime = curTime;

	if (_roundValues.timeForNextDramaticDrum > 0) _roundValues.timeForNextDramaticDrum -= deltaTime;

    _roundValues.currentMaxBombs += ROUND_MAX_BOMBS_INCREMENT * deltaTime;
    _roundValues.timeToSpawnNextBomb -= deltaTime;
    if (_roundValues.timeToSpawnNextBomb <= 0) // SPAWN BOMB
    {
        _roundValues.nextBombSpawnTime = max(_roundValues.nextBombSpawnTime - ROUND_BOMB_SPAWN_TIME_INCREMENT, ROUND_BOMB_SPAWN_TIME_MIN);
        _roundValues.timeToSpawnNextBomb = _roundValues.nextBombSpawnTime;

        if (_bombGameObjects.size() < (unsigned int)_roundValues.currentMaxBombs && _bombGameObjects.size() < (unsigned int)GetDeviceMaxBombs())
        {
            SpawnBombRound();
        }
    }

    HandleBombGrab();
    HandlePipeGrab();

	_pauseButtonHovered = CheckCollisionPointRec(GetWorldMousePos(), GetPauseButtonRect()) && _grabbedBomb == nullptr && _grabbedPipe == nullptr;
	if (_pauseButtonHovered != _pauseButtonHoveredLastFrame) audioManager->PlayBombGrabbedSound(0);

    if (IsPauseButtonPressed()) 
    { 
		if (_grabbedBomb != nullptr && !_grabbedBomb->isMarkedForDestroy()) _grabbedBomb->LetGo(GetBombReleasedState(_grabbedBomb));
		if (_grabbedPipe != nullptr && !_grabbedPipe->isMarkedForDestroy()) _grabbedPipe->LetGo();

		_grabbedBomb = nullptr;
		_grabbedPipe = nullptr;

        _state = ROUND_PAUSED; 
        _pauseState = PAUSE_NONE;

        _pauseButtonHovered = false;
        _pauseButtonHoveredLastFrame = false;

		_pauseDecoVariation = GetRandomValue(0, PAUSE_MENU_DECO_VARIANTS - 1);

        audioManager->PlayBombReleasedBombHouseSound(0);
        audioManager->PauseMusic(); 
    }

    _pauseButtonHoveredLastFrame = _pauseButtonHovered;
}

void GameManager::UpdateRoundPaused(const float deltaTime)
{
	Vector2 mousePos = GetWorldMousePos();

	const Rectangle resumeButtonRect = { PAUSE_MENU_SPRITE_POSITION_X + PAUSE_MENU_RESUME_BUTTON_OFFSET_X, PAUSE_MENU_SPRITE_POSITION_Y + PAUSE_MENU_RESUME_BUTTON_OFFSET_Y, PAUSE_MENU_BUTTON_SIZE_X, PAUSE_MENU_BUTTON_SIZE_Y };
	const Rectangle exitButtonRect = { PAUSE_MENU_SPRITE_POSITION_X + PAUSE_MENU_EXIT_BUTTON_OFFSET_X, PAUSE_MENU_SPRITE_POSITION_Y + PAUSE_MENU_EXIT_BUTTON_OFFSET_Y, PAUSE_MENU_BUTTON_SIZE_X, PAUSE_MENU_BUTTON_SIZE_Y };
    
    if (CheckCollisionPointRec(mousePos, resumeButtonRect))
    {
        if (_pauseState != PAUSE_HOLD_RESUME) { _pauseState = PAUSE_HOLD_RESUME; audioManager->PlayBombGrabbedSound(0); }
    }
	else if (CheckCollisionPointRec(mousePos, exitButtonRect))
	{
		if (_pauseState != PAUSE_HOLD_EXIT) { _pauseState = PAUSE_HOLD_EXIT; audioManager->PlayBombGrabbedSound(0); }
	}
    else
    {
		if (_pauseState != PAUSE_NONE) { _pauseState = PAUSE_NONE; audioManager->PlayBombGrabbedSound(0); }
    }

	if (IsPauseButtonPressed() || (_pauseState == PAUSE_HOLD_RESUME && !_currentPressed && _prevPressed))
    { 
		_pauseResumeTicks = PAUSE_RESUME_COUNTDOWN_TICKS + 1; // +1 because the first tick is instant
        _pauseResumeTimer = PAUSE_RESUME_COUNTDOWN_TICK_TIME;
        _state = ROUND_RESUMING; 

        _pauseButtonHovered = false;
        _pauseButtonHoveredLastFrame = false;

        audioManager->PlayBombReleasedBombHouseSound(0);
    }

    if (_pauseState == PAUSE_HOLD_EXIT && !_currentPressed && _prevPressed)
    {
        audioManager->StopMusic();
        audioManager->PlayBombReleasedBombHouseSound(0);

        _bombHouseTop->GameOver();   // Cancels transitions
        _bombHouseBottom->GameOver();

        _pipe->GameOver();

        StartMainMenu();
    }

    _prevPressed = _currentPressed;
}

void GameManager::UpdateRoundResuming(const float deltaTime)
{
	_pauseResumeTimer += deltaTime;
    if (_pauseResumeTimer >= PAUSE_RESUME_COUNTDOWN_TICK_TIME)
    {
		_pauseResumeTimer = 0.0f;
		_pauseResumeTicks--;

		audioManager->PlayBombHouseTransitionFlashSound();

		if (_pauseResumeTicks <= 0) 
        {
             _state = ROUND; 
             audioManager->ResumeMusic(); 
             _musicPrevTime = audioManager->GetMusicTime();

             _pauseButtonHovered = false;
             _pauseButtonHoveredLastFrame = false;
        }
    }
}

void GameManager::UpdateGameOver(const float deltaTime)
{
    _gameOverOverlay->Update(deltaTime);

    _gameOverRestartTimer -= deltaTime;
    if (_gameOverRestartTimer <= 0 && _bombGameObjects.empty() && _explosionGameObjects.empty())
    {
        if (_roundValues.canRevive) StartReviveDecision();
        else StartPointTally();
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
        if (_pointTallyCounterLast % POINT_TALLY_SOUND_POINT_INTERVAL == 0) audioManager->PlayPointTallySound();
	}

	if (_prevPressed && _currentPressed) _pointTallySkipClicks++;
	if (_pointTallySkipClicks >= POINT_TALLY_SKIP_CLICKS) _pointTallyCounter = (float)_roundValues.score;

	if (_pointTallyCounter >= _roundValues.score)
	{
		if (_roundValues.score > _highScore)
		{
			_highScore = _roundValues.score;
			SaveData();
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
    _currentPressed = IsMouseButtonDown(0);

    audioManager->Update(deltaTime);

    switch (_state)
    {
        case MAIN_MENU: 
        {
            UpdateMainMenu(deltaTime);
            break;
        }
        case MENU_INFO:
        {
            UpdateMenuInfo(deltaTime);
            break;
        }
        case ROUND:
        {
            UpdateRound(deltaTime);
            break;
        }
		case ROUND_PAUSED:
		{
			UpdateRoundPaused(deltaTime);
            return;
			break;
		}
		case ROUND_RESUMING:
		{
			UpdateRoundResuming(deltaTime);
            return;
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

    _pipe->Update(deltaTime);

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

    _prevPressed = _currentPressed;
}

void GameManager::RenderMenuInfo()
{
    // Bottom (Buttons)
    DrawTexturePro(
        _sprMenuInfo, { float((int)_menuInfoState * MENU_INFO_SPRITE_SIZE_X), MENU_INFO_INFO_SIZE_Y, MENU_INFO_SPRITE_SIZE_X, MENU_INFO_SPRITE_SIZE_Y - MENU_INFO_INFO_SIZE_Y },
        { MENU_INFO_SPRITE_POSITION_X, MENU_INFO_SPRITE_POSITION_Y + MENU_INFO_INFO_SIZE_Y, MENU_INFO_SPRITE_SIZE_X, MENU_INFO_SPRITE_SIZE_Y - MENU_INFO_INFO_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    // Top (Info)
    DrawTexturePro(
        _sprMenuInfo, { float(_menuInfoPage * MENU_INFO_SPRITE_SIZE_X), 0, MENU_INFO_SPRITE_SIZE_X, MENU_INFO_INFO_SIZE_Y },
        { MENU_INFO_SPRITE_POSITION_X, MENU_INFO_SPRITE_POSITION_Y, MENU_INFO_SPRITE_SIZE_X, MENU_INFO_INFO_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    if (DEBUG_MENU_INFO_DRAW_BUTTON_BOUNDS)
    {
        DrawRectangleLines(MENU_INFO_SPRITE_POSITION_X + MENU_INFO_BUTTON_NEXT_OFFSET_X, MENU_INFO_SPRITE_POSITION_Y + MENU_INFO_BUTTON_NEXT_OFFSET_Y, MENU_INFO_BUTTON_SIZE_X, MENU_INFO_BUTTON_SIZE_Y, GREEN);
        DrawRectangleLines(MENU_INFO_SPRITE_POSITION_X + MENU_INFO_BUTTON_EXIT_OFFSET_X, MENU_INFO_SPRITE_POSITION_Y + MENU_INFO_BUTTON_EXIT_OFFSET_Y, MENU_INFO_BUTTON_SIZE_X, MENU_INFO_BUTTON_SIZE_Y, GREEN);
    }
}

void GameManager::RenderRoundPaused()
{
    // Bottom (Buttons)
	DrawTexturePro(
		_sprPauseMenu, { float(_pauseState * PAUSE_MENU_SPRITE_SIZE_X), PAUSE_MENU_DECO_SIZE_Y, PAUSE_MENU_SPRITE_SIZE_X, PAUSE_MENU_SPRITE_SIZE_Y - PAUSE_MENU_DECO_SIZE_Y },
		{ PAUSE_MENU_SPRITE_POSITION_X, PAUSE_MENU_SPRITE_POSITION_Y + PAUSE_MENU_DECO_SIZE_Y, PAUSE_MENU_SPRITE_SIZE_X, PAUSE_MENU_SPRITE_SIZE_Y - PAUSE_MENU_DECO_SIZE_Y },
		{ 0, 0 }, 0.0f, WHITE
	);

    // Top (Deco)
    DrawTexturePro(
        _sprPauseMenu, { float(_pauseDecoVariation * PAUSE_MENU_SPRITE_SIZE_X), 0, PAUSE_MENU_SPRITE_SIZE_X, PAUSE_MENU_DECO_SIZE_Y },
        { PAUSE_MENU_SPRITE_POSITION_X, PAUSE_MENU_SPRITE_POSITION_Y, PAUSE_MENU_SPRITE_SIZE_X, PAUSE_MENU_DECO_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    if (DEBUG_PAUSE_MENU_DRAW_BUTTON_BOUNDS)
    {
		DrawRectangleLines(PAUSE_MENU_SPRITE_POSITION_X + PAUSE_MENU_RESUME_BUTTON_OFFSET_X, PAUSE_MENU_SPRITE_POSITION_Y + PAUSE_MENU_RESUME_BUTTON_OFFSET_Y, PAUSE_MENU_BUTTON_SIZE_X, PAUSE_MENU_BUTTON_SIZE_Y, GREEN);
		DrawRectangleLines(PAUSE_MENU_SPRITE_POSITION_X + PAUSE_MENU_EXIT_BUTTON_OFFSET_X, PAUSE_MENU_SPRITE_POSITION_Y + PAUSE_MENU_EXIT_BUTTON_OFFSET_Y, PAUSE_MENU_BUTTON_SIZE_X, PAUSE_MENU_BUTTON_SIZE_Y, GREEN);
    }
}

void GameManager::RenderRoundResuming()
{
    DrawTexturePro(
        _sprPauseButton, { float(2 * PAUSE_BUTTON_SPRITE_SIZE_X), 0, PAUSE_BUTTON_SPRITE_SIZE_X, PAUSE_BUTTON_SPRITE_SIZE_Y },
        { -PAUSE_BUTTON_SPRITE_SIZE_X / 2.0f, PAUSE_RESUMING_SPRITE_POSITION_Y, PAUSE_BUTTON_SPRITE_SIZE_X, PAUSE_BUTTON_SPRITE_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    DrawResumeNumber(_pauseResumeTicks, { -SCREEN_NUMBER_SPRITE_WIDTH / 2.0f, PAUSE_RESUMING_SPRITE_POSITION_Y + SMALL_SCREEN_NUMBER_POSITION_OFFSET_Y }, WHITE);
}

void GameManager::Render(const float deltaTime) 
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    float shakeIntensity = _screenShakeTrauma * _screenShakeTrauma;

    float zoomForHeight = (float)height / MAP_COORD_SIZE;
    float zoomForWidth = (float)width / MAP_COORD_HORIZONTAL_MIN_VIEWABLE_SIZE;

    _cam.target = { 0, 0 };
    _cam.offset = { width / 2.0f, height / 2.0f }; // Screen center
    _cam.rotation = sinf(_screenShakePhase) * SCREEN_SHAKE_MAX_ROTATION_DEGREES * shakeIntensity;
    _cam.zoom = fminf(zoomForHeight, zoomForWidth);

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
        _sprMapMG, { 0, 0, MAP_MG_SPRITE_SIZE_X, MAP_MG_SPRITE_SIZE_Y },
        { MAP_BG_COORD_RADIUS_X - MAP_MG_COORD_SIZE_X,
          -MAP_BG_COORD_RADIUS_Y,
          MAP_MG_COORD_SIZE_X, MAP_MG_COORD_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    sort(_bombGameObjects.begin(), _bombGameObjects.end(),
         [](const std::unique_ptr<Bomb>& a, const std::unique_ptr<Bomb>& b) {
            return Bomb::BombLayerSort(a.get(), b.get());
         });


    for (auto& o : _bombGameObjects) o->Render(deltaTime);
    _bombHouseTop->RenderScreen();
    for (auto& o : _explosionGameObjects) o->Render(deltaTime);

    if (DEBUG_DRAW_MAP_BOUNDS)
    {
		Vector2 boundsH = GetHorizontalBounds();

        DrawRectangleLines(boundsH.x, -500, (boundsH.y - boundsH.x), 1000, GREEN);
        DrawLine(boundsH.x, BOMBHOUSE_COORD_BOT_VER_POS, boundsH.y, BOMBHOUSE_COORD_BOT_VER_POS, BLUE);
        DrawLine(boundsH.x, BOMBHOUSE_COORD_TOP_VER_POS, boundsH.y, BOMBHOUSE_COORD_TOP_VER_POS, BLUE);

        DrawRectangleLines(boundsH.x, MAP_COORD_VER_MIN, (boundsH.y - boundsH.x), (MAP_COORD_VER_MAX - MAP_COORD_VER_MIN), WHITE);

		DrawRectangleLines(-MAP_COORD_HORIZONTAL_MIN_VIEWABLE_SIZE / 2.0f, -MAP_COORD_SIZE / 2.0f, MAP_COORD_HORIZONTAL_MIN_VIEWABLE_SIZE, MAP_COORD_SIZE, RED);
    }

    // Map fg
    DrawTexturePro(
        _sprMapFG, { 0, 0, MAP_FG_SPRITE_SIZE_X, MAP_FG_SPRITE_SIZE_Y },
        { -MAP_BG_COORD_RADIUS_X,
          MAP_BG_COORD_RADIUS_Y - MAP_FG_COORD_SIZE_Y,
          MAP_FG_COORD_SIZE_X, MAP_FG_COORD_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    _bombHouseBottom->RenderScreen();

    _pipe->Render(deltaTime);

    if (_grabbedBomb != nullptr && !_grabbedBomb->isMarkedForDestroy()) _grabbedBomb->Render(deltaTime); // Render grabbed bomb on top of everything

    DrawTexturePro(
        _sprSmallScreen, { 0, 0, SMALL_SCREEN_SPRITE_WIDTH, SMALL_SCREEN_SPRITE_HEIGHT },
        { GetSmallScreenPosX(), GetSmallScreenPosY(), SMALL_SCREEN_SPRITE_WIDTH, SMALL_SCREEN_SPRITE_HEIGHT},
        { 0, 0 }, 0.0f, WHITE
    );

    int numberToDraw = _roundValues.score;
	Color numberColor = WHITE;
	if (_state == MAIN_MENU || _state == MENU_INFO) { numberToDraw = _highScore; numberColor = SMALL_NUMBER_YELLOW_COLOR; }
	if ((_state == ROUND || _state == ROUND_PAUSED || _state == ROUND_RESUMING || _state == REVIVE_DECISION) && _roundValues.score > _highScore) numberColor = SMALL_NUMBER_YELLOW_COLOR;
    if (_state == GAME_OVER_CUTSCENE || _state == GAME_OVER || _state == POINT_TALLY || _state == POINT_TALLY_DONE) numberToDraw = -1; // -1 to show DEAD
    DrawScreenNumber(numberToDraw, 
        { GetSmallScreenPosX() + SMALL_SCREEN_NUMBER_POSITION_OFFSET_X,
        GetSmallScreenPosY() + SMALL_SCREEN_NUMBER_POSITION_OFFSET_Y }, numberColor);

	if (_state == POINT_TALLY || _state == POINT_TALLY_DONE)
	{
		DrawScreenNumber((int)_pointTallyCounter, { BOMBHOUSE_SCREEN_TOP_COORD_X + BOMBHOUSE_SCREEN_TEXT_OFFSET_X, BOMBHOUSE_SCREEN_TOP_COORD_Y }, WHITE);
		DrawScreenNumber((int)_highScore, { BOMBHOUSE_SCREEN_BOT_COORD_X + BOMBHOUSE_SCREEN_TEXT_OFFSET_X, BOMBHOUSE_SCREEN_BOT_COORD_Y }, SMALL_NUMBER_YELLOW_COLOR);
	}

    // Pause button
    if (_state == ROUND)
    {
        DrawTexturePro(
            _sprPauseButton, { float((int)_pauseButtonHovered * PAUSE_BUTTON_SPRITE_SIZE_X), 0, PAUSE_BUTTON_SPRITE_SIZE_X, PAUSE_BUTTON_SPRITE_SIZE_Y},
            GetPauseButtonRect(),
            { 0, 0 }, 0.0f, WHITE
        );
    }

	// Gradient and black rectangles on map limits to hide the edges of the map
    if (width >= height)
    {
        DrawRectangleGradientH(-MAP_BG_COORD_RADIUS_X, -MAP_COORD_RADIUS, 60, MAP_COORD_SIZE, BLACK, { 0, 0, 0, 0 });
        DrawRectangleGradientH(MAP_BG_COORD_RADIUS_X - 60, -MAP_COORD_RADIUS, 60, MAP_COORD_SIZE, { 0, 0, 0, 0 }, BLACK);

        Vector2 boundsH = GetHorizontalBounds();
        const float maskExtent = MAP_BG_COORD_SIZE_X;
        const float maskY = -MAP_BG_COORD_RADIUS_Y;
        const float maskHeight = MAP_BG_COORD_SIZE_Y;
        DrawRectangleRec({ -maskExtent, maskY, maskExtent + boundsH.x, maskHeight }, BLACK);
        DrawRectangleRec({ boundsH.y,   maskY, maskExtent - boundsH.y, maskHeight }, BLACK);
    }

    _gameOverOverlay->Render(deltaTime);

    if (_state == MENU_INFO) RenderMenuInfo();
	else if (_state == ROUND_PAUSED) RenderRoundPaused();
	else if (_state == ROUND_RESUMING) RenderRoundResuming();

    if (DEBUG_DRAW_FPS) DrawFPS(0, -500);

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

    _reviveHouseTopType = _bombHouseTop->GetType();
    _reviveHouseBottomType = _bombHouseBottom->GetType();

    _gameOverBomb = obj;

    if (_grabbedBomb != nullptr)
    {
        if (!_grabbedBomb->isMarkedForDestroy()) 
        {
            _grabbedBomb->LetGo(GetBombReleasedState(_grabbedBomb));
        }
        _grabbedBomb = nullptr;
	}

    if (_grabbedPipe != nullptr)
    {
        _grabbedPipe->LetGo();
        _grabbedPipe = nullptr;
    }

    for (auto& o : _bombGameObjects) o->GameOver();

    _bombHouseTop->GameOver();
    _bombHouseBottom->GameOver();

    _pipe->GameOver();

    _gameOverOverlay->SetPosition(obj->GetPosition());
    _gameOverOverlay->GameOver();
}

bool GameManager::IsGameOverCutscene() const
{
    return _state == GAME_OVER_CUTSCENE;
}

bool GameManager::IsPauseButtonPressed() const
{
    if (_state == ROUND && !_currentPressed && _prevPressed && _pauseButtonHoveredLastFrame) return true;
    return (_state == ROUND || _state == ROUND_PAUSED) && IsKeyPressed(KEY_ESCAPE);
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
    if (score > 9999) 
    {
		char s[5];
		sprintf(s, "%d", score);
		DrawTextEx(GetFontDefault(), s, { position.x, position.y + 8 }, 32, 10, color);
        return;
    }

    int digits[4] = { 0 };

    if (score == -1) // Show DEAD
    {
        digits[0] = 10;
        digits[1] = 11;
        digits[2] = 12;
        digits[3] = 10;
    }
    else
    {
        digits[0] = score / 1000;
        digits[1] = (score / 100) % 10;
        digits[2] = (score / 10) % 10;
        digits[3] = score % 10;
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

void GameManager::DrawResumeNumber(const int number, const Vector2 position, const Color color) const
{
    if (number > 9 || number < 0) return;

    int digit = number % 10;
    if (digit > 3) digit = 3;
    DrawTexturePro(
        _sprScreenNumbers, { (float)(digit * SCREEN_NUMBER_SPRITE_WIDTH), 0, SCREEN_NUMBER_SPRITE_WIDTH, SCREEN_NUMBER_SPRITE_HEIGHT },
        { position.x, position.y, SCREEN_NUMBER_SPRITE_WIDTH, SCREEN_NUMBER_SPRITE_HEIGHT },
        { 0, 0 }, 0.0f, color
    );
}

int GameManager::GetBombReleasedState(Bomb* obj)
{
    Vector2 bombPos = obj->GetPosition();

    if (bombPos.y >= BOMBHOUSE_COORD_BOT_VER_POS) return BOMB_RELEASED_BOT;
    if (bombPos.y <= BOMBHOUSE_COORD_TOP_VER_POS) return BOMB_RELEASED_TOP;

    return BOMB_RELEASED_DEF;
}

void GameManager::BombEntered(Bomb* obj, int _placedDirection)
{
    if (_state == MAIN_MENU) 
    {
        if (_placedDirection == BOMB_PLACED_TOP) StartGame();
        else StartMenuInfo();
        return;
	}
    else if (_state == REVIVE_DECISION)
    {
        if (_placedDirection == BOMB_PLACED_TOP) StartGameRevive();
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

            if (_roundValues.score % 100 == 0) audioManager->PlayApplauseSound();
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

void GameManager::SpawnBombRound()
{
    float verticalPos = (float)GetRandomValue(MAP_COORD_VER_MIN, MAP_COORD_VER_MAX);
    int spawnPos = GetRandomValue(0, 1);

    if (spawnPos == 1) InstantiateBomb(std::make_unique<Bomb>(Vector2{ -GetBombSpawnPos(), verticalPos }, 0, 150, GetNewBombType()));
    else InstantiateBomb(std::make_unique<Bomb>(Vector2{ GetBombSpawnPos(), verticalPos }, 0, 150, GetNewBombType()));
}

void GameManager::HandleBombGrab()
{
    if (_grabbedPipe != nullptr) return;

    if (_grabbedBomb == nullptr && _currentPressed && !_prevPressed)
    {
        TryGrabBomb(GetWorldMousePos());
        if (_grabbedBomb != nullptr) 
        {
            audioManager->PlayBombGrabbedSound(GetPan(_grabbedBomb->GetPosition()));
            _grabbedBomb->Grab();
        }
    }
    else if (_grabbedBomb != nullptr && IsMouseButtonReleased(0))
    {
        if (!_grabbedBomb->isMarkedForDestroy()) 
        {
            int letGoState = _grabbedBomb->LetGo(GetBombReleasedState(_grabbedBomb));

            if (letGoState == BOMB_PLACED_TOP || letGoState == BOMB_PLACED_BOT) audioManager->PlayBombReleasedBombHouseSound(GetPan(_grabbedBomb->GetPosition()));
            else audioManager->PlayBombReleasedMetalSound(GetPan(_grabbedBomb->GetPosition()));

            if ( (letGoState == BOMB_PLACED_TOP && !_bombHouseTop->GetIsBombEnteredTypeValid(_grabbedBomb->GetType())) ||
                (letGoState == BOMB_PLACED_BOT && !_bombHouseBottom->GetIsBombEnteredTypeValid(_grabbedBomb->GetType())))
            {
                if (_roundValues.timeForNextDramaticDrum <= 0)
                {
                    _grabbedBomb->SnapToBombHouseMin();
                    audioManager->PlayDramaticDrum();
                    _roundValues.timeForNextDramaticDrum = ROUND_TIME_FOR_DRAMATIC_DRUM;
                    AddScreenShake(DRAMATIC_DRUM_SCREEN_SHAKE_TIME);
                }
            }
        }
        _grabbedBomb = nullptr;
    }
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

int GameManager::GetDeviceMaxBombs() const
{
    float visibleWidth = GetHorizontalBounds().y;
    float t = visibleWidth / (float)MAP_COORD_HOR_MAX;

    return (int)Lerp((float)ROUND_DEVICE_MAX_BOMBS_MIN, (float)ROUND_DEVICE_MAX_BOMBS_MAX, t);
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

void GameManager::HandlePipeGrab()
{
    if (_grabbedBomb != nullptr) return;

    if (_grabbedPipe == nullptr && _currentPressed && !_prevPressed)
    {
        if (_pipe->WasClicked(GetWorldMousePos())) _grabbedPipe = _pipe.get();

        if (_grabbedPipe != nullptr) 
        {
            audioManager->PlayBombGrabbedSound(GetPan(_grabbedPipe->GetPosition()));
            _grabbedPipe->Grab();
        }
    }
    else if (_grabbedPipe != nullptr && IsMouseButtonReleased(0))
    {
        if (!_grabbedPipe->isMarkedForDestroy()) 
        {
            _grabbedPipe->LetGo();

            audioManager->PlayBombReleasedMetalSound(GetPan(_grabbedPipe->GetPosition()));
        }
        _grabbedPipe = nullptr;
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

float GameManager::GetScreenBottomWorld() const
{
    return (GetScreenHeight() / 2.0f) / _cam.zoom;
}

float GameManager::GetScreenTopWorld() const
{
    return -(GetScreenHeight() / 2.0f) / _cam.zoom;
}

float GameManager::GetScreenRightWorld() const
{
    return (GetScreenWidth() / 2.0f) / _cam.zoom;
}

float GameManager::GetSmallScreenPosX() const
{
    return GetScreenRightWorld() - SMALL_SCREEN_PIXEL_OFFSET_X / _cam.zoom - SMALL_SCREEN_SPRITE_WIDTH;
}

float GameManager::GetSmallScreenPosY() const
{
    return GetScreenBottomWorld() - SMALL_SCREEN_PIXEL_OFFSET_Y / _cam.zoom - SMALL_SCREEN_SPRITE_HEIGHT;
}

Rectangle GameManager::GetPauseButtonRect() const
{
    float rightEdge = GetSmallScreenPosX() + SMALL_SCREEN_SPRITE_WIDTH;
    float x = rightEdge - PAUSE_BUTTON_SPRITE_SIZE_X;
    float y = GetSmallScreenPosY() - PAUSE_BUTTON_PIXEL_OFFSET_Y / _cam.zoom - PAUSE_BUTTON_SPRITE_SIZE_Y;
    return Rectangle{ x, y, PAUSE_BUTTON_SPRITE_SIZE_X, PAUSE_BUTTON_SPRITE_SIZE_Y };
}

float GameManager::GetPan(const Vector2& position) const
{
    Vector2 bounds = GetHorizontalBounds();
    float pan = (position.x - bounds.x) / (bounds.y - bounds.x);
    return 1.0f - pan;
}

float GameManager::GetBombExplosionSoundVolume() const
{
    int explosionCount = (int)_explosionGameObjects.size();

    float volume = (explosionCount <= 0) ? 1.0f : (0.5f / (float)explosionCount) + 0.5f;

    return min(volume, 1.0f);
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

bool GameManager::IsPaused() const
{
	return _state == ROUND_PAUSED || _state == ROUND_RESUMING;
}

bool GameManager::PlayerExited() const
{
    return _playerExited;
}

void GameManager::GetSaveData()
{
    std::string path = std::string(GetApplicationDirectory()) + SAVE_FILE_PATH;

    if (!FileExists(path.c_str()))
    {
        _highScore = HIGHSCORE_DEFAULT;
        return;
    }

    char* data = LoadFileText(path.c_str());
    if (data == nullptr)
    {
        _highScore = HIGHSCORE_DEFAULT;
        return;
    }

    _highScore = atoi(data);
    UnloadFileText(data);
}

void GameManager::SaveData()
{
    std::string path = std::string(GetApplicationDirectory()) + SAVE_FILE_PATH;

    char buffer[16];
    sprintf(buffer, "%d", _highScore);
    SaveFileText(path.c_str(), buffer);
}