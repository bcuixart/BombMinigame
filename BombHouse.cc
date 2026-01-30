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
}

void BombHouse::SetType(BombType t, bool isFirstTime)
{
	_bombTypeOld = (isFirstTime) ? BOMB_INVALID : _bombType; 
	_bombType = t;

	if (!isFirstTime) _lenienceTime = BOMBHOUSE_LENIENCE_TIME;
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

	float animProgress = BOMBHOUSE_ANIMATION_SPEED * deltaTime;
	_animationFrame += (_houseType == BOMBHOUSE_TOP) ? animProgress : -animProgress;
	if (int(_animationFrame) >= BOMBHOUSE_ANIMATION_FRAMES) _animationFrame = 0;
}

void BombHouse::RenderScreen() const
{
	Rectangle dest;

	if (_houseType == BOMBHOUSE_BOTTOM) dest = { BOMBHOUSE_SCREEN_BOT_COORD_X, BOMBHOUSE_SCREEN_BOT_COORD_Y, BOMBHOUSE_SCREEN_COORD_SIZE, BOMBHOUSE_SCREEN_COORD_SIZE };
	else dest = { BOMBHOUSE_SCREEN_TOP_COORD_X, BOMBHOUSE_SCREEN_TOP_COORD_Y, BOMBHOUSE_SCREEN_COORD_SIZE, BOMBHOUSE_SCREEN_COORD_SIZE };
	Vector2 origin = { BOMBHOUSE_SCREEN_COORD_SIZE / 2, BOMBHOUSE_SCREEN_COORD_SIZE / 2 };	

	int screenSprite = _bombType;
	int screenSpriteOld = _bombTypeOld;
	if (_lenienceTime < BOMBHOUSE_LENIENCE_TIME && _lenienceTime > 0) // Transition
	{
		// Draw old complete at first
		DrawTexturePro
		(	GameManager::instance->sprBombHouseScreen,
			{ float(screenSpriteOld * BOMBHOUSE_SCREEN_SPRITE_SIZE), 0, BOMBHOUSE_SCREEN_SPRITE_SIZE, BOMBHOUSE_SCREEN_SPRITE_SIZE }, // SOURCE
			dest, origin, 0, WHITE
		);	

		// Then interpolate new sprite and draw on top
		float t = 1.0f - (_lenienceTime / BOMBHOUSE_LENIENCE_TIME);
		
		const float PIXEL_STEP = 8.0f;
		const float TOTAL_STEPS = BOMBHOUSE_SCREEN_SPRITE_SIZE / PIXEL_STEP;
		float quantizedT = floor(t * TOTAL_STEPS) / TOTAL_STEPS;
		
		float spriteWidth = quantizedT * BOMBHOUSE_SCREEN_SPRITE_SIZE;
		float coordWidth = quantizedT * BOMBHOUSE_SCREEN_COORD_SIZE;

		if (_houseType == BOMBHOUSE_BOTTOM) dest = { BOMBHOUSE_SCREEN_BOT_COORD_X, BOMBHOUSE_SCREEN_BOT_COORD_Y, coordWidth, BOMBHOUSE_SCREEN_COORD_SIZE };
		else dest = { BOMBHOUSE_SCREEN_TOP_COORD_X, BOMBHOUSE_SCREEN_TOP_COORD_Y, coordWidth, BOMBHOUSE_SCREEN_COORD_SIZE };
		
		DrawTexturePro
		(	GameManager::instance->sprBombHouseScreen,
			{ float(screenSprite * BOMBHOUSE_SCREEN_SPRITE_SIZE), 0, spriteWidth, BOMBHOUSE_SCREEN_SPRITE_SIZE }, // SOURCE
			dest, origin, 0, WHITE
		);
	}
	else // Regular
	{
		DrawTexturePro
		(	GameManager::instance->sprBombHouseScreen,
			{ float(screenSprite * BOMBHOUSE_SCREEN_SPRITE_SIZE), 0, BOMBHOUSE_SCREEN_SPRITE_SIZE, BOMBHOUSE_SCREEN_SPRITE_SIZE }, // SOURCE
			dest, origin, 0, WHITE
		);		
	}	
}

void BombHouse::Render(const float deltaTime) 
{
	Rectangle dest = { _position.x, _position.y, _scale, _scale };
	Vector2 origin = { _radius, _radius };
	DrawTexturePro
	(	GameManager::instance->sprBombHouse,
		{ float(int(_animationFrame) * BOMBHOUSE_SPRITE_SIZE), 0, BOMBHOUSE_SPRITE_SIZE, BOMBHOUSE_SPRITE_SIZE }, // SOURCE
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