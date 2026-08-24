#include "BombHouse.hh"
#include "GameManager.hh"

BombHouse::BombHouse(const Vector2 p, const float r, const float s, const BombHouseType h) : 
	GameObject(p, r, s)
{
	_houseType = h;
	
	_bombType = BOMB_INVALID;
	_bombTypeOld = BOMB_INVALID;

	_radius = s / 2;

	_animationFrame = 0;

	_lenienceTime = -1;
	_isTransitioning = false;
	_isScreenTransitionBright = false;
	_wasScreenTransitionBrightLastFrame = false;
	_screenFlashStartTime = 0;

	_screenSprite = _bombType;
	_screenSpriteOld = _bombTypeOld;
}

void BombHouse::SetType(BombType t, bool isFirstTime)
{
	_bombTypeOld = (isFirstTime) ? BOMB_INVALID : _bombType; 
	_bombType = t;

	_screenSpriteOld = _bombTypeOld;
	_screenSprite = _bombType;
	if (_bombType == BOMB_MENU) _screenSprite = (_houseType == BOMBHOUSE_TOP) ? BOMBHOUSE_SCREEN_SPRITE_INDEX_PLAY : BOMBHOUSE_SCREEN_SPRITE_INDEX_INFO;
	else if (_bombType == BOMB_REVIVE) _screenSprite = (_houseType == BOMBHOUSE_TOP) ? BOMBHOUSE_SCREEN_SPRITE_INDEX_REVIVE : BOMBHOUSE_SCREEN_SPRITE_INDEX_NOT_REVIVE;
	else if (_bombType == BOMB_POINT_TALLY) _screenSprite = (_houseType == BOMBHOUSE_TOP) ? BOMBHOUSE_SCREEN_SPRITE_INDEX_SCORE : BOMBHOUSE_SCREEN_SPRITE_INDEX_HIGHSCORE;

	if (!isFirstTime) 
	{ 
		_lenienceTime = BOMBHOUSE_LENIENCE_TIME; 
		_isTransitioning = true; 
	
		float musicTime = GameManager::instance->audioManager->GetMusicTime();
		float cycleDuration = BOMBHOUSE_SCREEN_TRANSITION_FLASH_BEAT_DURATION * BOMBHOUSE_SCREEN_TRANSITION_FLASH_CYCLE_BEATS;
		_screenFlashStartTime = ceilf(musicTime / cycleDuration) * cycleDuration;
	}
}

BombType BombHouse::GetType() const
{
	return _bombType;
}

bool BombHouse::GetIsBombEnteredTypeValid(BombType t) const 
{
	if (_lenienceTime > 0 && t == _bombTypeOld) return true;
	return (t == _bombType);
}

void BombHouse::Update(float deltaTime)
{
	_lenienceTime -= deltaTime;

	_isTransitioning = (_lenienceTime > 0 && _lenienceTime < BOMBHOUSE_LENIENCE_TIME);

	if (_isTransitioning) 
	{
		float musicTime = GameManager::instance->audioManager->GetMusicTime();
		float cycleDuration = BOMBHOUSE_SCREEN_TRANSITION_FLASH_BEAT_DURATION * BOMBHOUSE_SCREEN_TRANSITION_FLASH_CYCLE_BEATS;

		_isScreenTransitionBright = false;

		if (musicTime >= _screenFlashStartTime)
		{
			float elapsedSinceFlashStart = musicTime - _screenFlashStartTime;
			int cycleIndex = int(elapsedSinceFlashStart / cycleDuration);
			if (cycleIndex < BOMBHOUSE_SCREEN_TRANSITION_FLASH_COUNT)
			{
				float beatPhase = fmodf(elapsedSinceFlashStart, cycleDuration) / cycleDuration;
				_isScreenTransitionBright = beatPhase < BOMBHOUSE_SCREEN_TRANSITION_FLASH_DUTY_CYCLE;
			}
		}

		if (_isScreenTransitionBright && !_wasScreenTransitionBrightLastFrame) GameManager::instance->audioManager->PlayBombHouseTransitionFlashSound();
	}
	else { _isScreenTransitionBright = false; _wasScreenTransitionBrightLastFrame = false; }

	float animProgress = BOMBHOUSE_ANIMATION_SPEED * deltaTime;
	_animationFrame += (_houseType == BOMBHOUSE_TOP) ? animProgress : -animProgress;
	if (_animationFrame >= BOMBHOUSE_ANIMATION_FRAMES) _animationFrame -= BOMBHOUSE_ANIMATION_FRAMES;
	if (_animationFrame < 0.0f) _animationFrame += BOMBHOUSE_ANIMATION_FRAMES;

	_wasScreenTransitionBrightLastFrame = _isScreenTransitionBright;
}

void BombHouse::RenderScreen() const
{
	Rectangle dest;

	if (_houseType == BOMBHOUSE_BOTTOM) dest = { BOMBHOUSE_SCREEN_BOT_COORD_X, BOMBHOUSE_SCREEN_BOT_COORD_Y, BOMBHOUSE_SCREEN_COORD_SIZE, BOMBHOUSE_SCREEN_COORD_SIZE };
	else dest = { BOMBHOUSE_SCREEN_TOP_COORD_X, BOMBHOUSE_SCREEN_TOP_COORD_Y, BOMBHOUSE_SCREEN_COORD_SIZE, BOMBHOUSE_SCREEN_COORD_SIZE };
	Vector2 origin = { BOMBHOUSE_SCREEN_COORD_SIZE / 2, BOMBHOUSE_SCREEN_COORD_SIZE / 2 };	

	if (_isTransitioning)
	{
		int screenSpriteOldFrame = (_isScreenTransitionBright && _screenSpriteOld < 4) ? _screenSpriteOld + 4 : _screenSpriteOld;
		int screenSpriteFrame    = (_isScreenTransitionBright && _screenSprite    < 4) ? _screenSprite    + 4 : _screenSprite;

		// Draw old complete at first
		DrawTexturePro
		(	GameManager::instance->sprBombHouseScreen,
			{ float(screenSpriteOldFrame * BOMBHOUSE_SCREEN_SPRITE_SIZE), 0, BOMBHOUSE_SCREEN_SPRITE_SIZE, BOMBHOUSE_SCREEN_SPRITE_SIZE },
			dest, origin, 0, WHITE
		);	

		// Then interpolate new sprite and draw on top
		float t = 1.0f - (_lenienceTime / BOMBHOUSE_LENIENCE_TIME);

		const float PIXEL_STEP = 8.0f;
		const float TOTAL_STEPS = BOMBHOUSE_SCREEN_SPRITE_SIZE / PIXEL_STEP;
		const float START_STEP = BOMBHOUSE_SCREEN_TRANSITION_START_PIXEL;

		float stepIndex = START_STEP + floor(t * (TOTAL_STEPS - START_STEP));
		float quantizedT = stepIndex / TOTAL_STEPS;

		float spriteWidth = quantizedT * BOMBHOUSE_SCREEN_SPRITE_SIZE;
		float coordWidth = quantizedT * BOMBHOUSE_SCREEN_COORD_SIZE;

		if (_houseType == BOMBHOUSE_BOTTOM) dest = { BOMBHOUSE_SCREEN_BOT_COORD_X, BOMBHOUSE_SCREEN_BOT_COORD_Y, coordWidth, BOMBHOUSE_SCREEN_COORD_SIZE };
		else dest = { BOMBHOUSE_SCREEN_TOP_COORD_X, BOMBHOUSE_SCREEN_TOP_COORD_Y, coordWidth, BOMBHOUSE_SCREEN_COORD_SIZE };
		
		DrawTexturePro
		(	GameManager::instance->sprBombHouseScreen,
			{ float(screenSpriteFrame * BOMBHOUSE_SCREEN_SPRITE_SIZE), 0, spriteWidth, BOMBHOUSE_SCREEN_SPRITE_SIZE },
			dest, origin, 0, WHITE
		);
	}
	else // Regular
	{
		DrawTexturePro
		(   GameManager::instance->sprBombHouseScreen,
			{ float(_screenSprite * BOMBHOUSE_SCREEN_SPRITE_SIZE) + 0.5f, 0.5f,
			  BOMBHOUSE_SCREEN_SPRITE_SIZE - 1.0f, BOMBHOUSE_SCREEN_SPRITE_SIZE - 1.0f },
			dest, origin, 0, WHITE
		);
	}
}

void BombHouse::Render(const float deltaTime) 
{
	Rectangle dest = { _position.x, _position.y, _scale, _scale * 2.0f };
	Vector2 origin = { _radius, _radius };
	DrawTexturePro
	(	GameManager::instance->sprBombHouse,
		{ float(int(_animationFrame) * BOMBHOUSE_SPRITE_SIZE_X), 0, BOMBHOUSE_SPRITE_SIZE_X, BOMBHOUSE_SPRITE_SIZE_Y }, // SOURCE
		dest, origin, 0, WHITE
	);

	if (DEBUG_BOMBHOUSE_TYPE_DRAW)
	{
		Color c;
		if (_bombType == BOMB_BLACK) c = BLACK;
		else if (_bombType == BOMB_RED) c = RED;
		else if (_bombType == BOMB_BLUE) c = BLUE;
		else if (_bombType == BOMB_GREEN) c = GREEN;
		DrawRectangle(_position.x + 100, _position.y, 100, 100, c);

		if (_lenienceTime < 0) return;
		if (_bombTypeOld == BOMB_BLACK) c = BLACK;
		else if (_bombTypeOld == BOMB_RED) c = RED;
		else if (_bombTypeOld == BOMB_BLUE) c = BLUE;
		else if (_bombTypeOld == BOMB_GREEN) c = GREEN;
		DrawRectangle(_position.x + 200, _position.y, 50, 50, c);
	}
}

void BombHouse::GameOver() 
{
	_lenienceTime = -1;
	_isTransitioning = false;
	_isScreenTransitionBright = false;
	_wasScreenTransitionBrightLastFrame = false;
}