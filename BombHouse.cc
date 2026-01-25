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

void BombHouse::Render(const float deltaTime) 
{
	Rectangle dest = { _position.x, _position.y, _scale, _scale };
	Vector2 origin = { _radius, _radius };
	DrawTexturePro
	(	GameManager::instance->sprBombHouse,
		{ float(int(_animationFrame) * BOMB_SPRITE_SIZE), float(0) * BOMB_SPRITE_SIZE, BOMB_SPRITE_SIZE, BOMB_SPRITE_SIZE }, // SOURCE
		dest, origin, 0, WHITE
	);

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