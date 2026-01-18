#include "Bomb.hh"
#include "GameManager.hh"

Bomb::Bomb(const Vector2 p, const float r, const float s) : 
	GameObject(p, r, s)
{
	_radius = s / 2;
	_radiusVisual = _radius * (float(BOMB_SPRITE_HITBOX_SIZE) / float(BOMB_SPRITE_SIZE));
	_grabbedScaleMultiplier = 1;

	_movementDirection = { 1, 1 };

	_type = (rand() % 2 == 0) ? BOMB_BLACK : BOMB_RED;
	_color = (_type == BOMB_BLACK) ? (Color) { 55, 55, 55, 255 } : (Color) { 200, 0, 0, 255 };
	_state = RANDOM_MOVEMENT;

	_timeToExplode = 30;

	_animationFrame = 0;

	_didGameOver = false;
	_collidedThisFrame = false;
}

BombType Bomb::GetType() const 
{
	return _type;
}

void Bomb::Update(float deltaTime)
{
	_animationFrame += ANIMATION_SPEED * deltaTime;
	if (int(_animationFrame) >= ANIMATION_FRAMES) _animationFrame = 0;

	_collidedThisFrame = false;

	switch (_state) 
	{
		case RANDOM_MOVEMENT: {
			Update_RandomMovement(deltaTime);
			break;
		}
		case GRABBED: {
			Update_Grabbed(deltaTime);
			break;
		}		
		case PLACED_TOP: {
			Update_PlacedTop(deltaTime);
			break;
		}
		case PLACED_BOTTOM: {
			Update_PlacedBottom(deltaTime);
			break;
		}
	}
}

void Bomb::Update_RandomMovement(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->GameOver();

	_grabbedScaleMultiplier = 1;

	_position.x += _movementDirection.x * BOMB_MOVEMENT_SPEED * deltaTime;
	_position.y += _movementDirection.y * BOMB_MOVEMENT_SPEED * deltaTime;

	// Flip at bounds
	if (_position.x < MAP_COORD_HOR_MIN) _movementDirection.x = 1;
	else if (_position.x > MAP_COORD_HOR_MAX) _movementDirection.x = -1;	
	if (_position.y < MAP_COORD_VER_MIN) _movementDirection.y = 1;
	else if (_position.y > MAP_COORD_VER_MAX) _movementDirection.y = -1;

	_animationIndex = BOMB_ANIM_INDEX_WALK_BOT_LEFT;
	if (_movementDirection.x == 1 && _movementDirection.y == 1) _animationIndex = BOMB_ANIM_INDEX_WALK_BOT_RIGHT;
	else if (_movementDirection.x == 1 && _movementDirection.y == -1) _animationIndex = BOMB_ANIM_INDEX_WALK_TOP_RIGHT;
	else if (_movementDirection.x == -1 && _movementDirection.y == -1) _animationIndex = BOMB_ANIM_INDEX_WALK_TOP_LEFT;

	if (!_didGameOver && IsMouseButtonPressed(0))
	{
		if (CheckCollisionPointCircle(GameManager::instance->GetWorldMousePos(), _position, _radiusVisual)) _state = GRABBED;
	}
}

void Bomb::Update_Grabbed(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->GameOver();

	_grabbedScaleMultiplier = BOMB_GRABBED_SCALE_INDEX;

	_animationIndex = BOMB_ANIM_INDEX_GRABBED;

	_position = GameManager::instance->GetWorldMousePos();

	if (!IsMouseButtonDown(0) || _didGameOver) {
		int releasedState = GameManager::instance->GetBombReleasedState(this);

		if (releasedState == BOMB_RELEASED_TOP) _state = PLACED_TOP;
		else if (releasedState == BOMB_RELEASED_BOT) _state = PLACED_BOTTOM;
		else _state = RANDOM_MOVEMENT;
	}
}

void Bomb::Update_PlacedTop(const float deltaTime) 
{
	_timeToExplode = -1;
	_animationIndex = BOMB_ANIM_INDEX_PLACED_TOP;

	_grabbedScaleMultiplier = 1;

	if (_position.x < BOMBHOUSE_COORD_HOR_MIN) _position.x = BOMBHOUSE_COORD_HOR_MIN;
	else if (_position.x > BOMBHOUSE_COORD_HOR_MAX) _position.x = BOMBHOUSE_COORD_HOR_MAX;

	_position.y -= BOMB_MOVEMENT_SPEED * deltaTime;

	// TODO: DESTROY
}

void Bomb::Update_PlacedBottom(const float deltaTime) 
{
	_timeToExplode = -1;
	_animationIndex = BOMB_ANIM_INDEX_PLACED_BOTTOM;

	_grabbedScaleMultiplier = 1;

	if (_position.x < BOMBHOUSE_COORD_HOR_MIN) _position.x = BOMBHOUSE_COORD_HOR_MIN;
	else if (_position.x > BOMBHOUSE_COORD_HOR_MAX) _position.x = BOMBHOUSE_COORD_HOR_MAX;

	_position.y += BOMB_MOVEMENT_SPEED * deltaTime;

	// TODO: DESTROY
}

void Bomb::Render(const float deltaTime) 
{
	Color col = _color;
	if (_timeToExplode < 5 && _timeToExplode > 4.5f) col = _blowUpIndicatorColor;
	else if (_timeToExplode < 4 && _timeToExplode > 3.5f) col = _blowUpIndicatorColor;
	else if (_timeToExplode < 3 && _timeToExplode > 2.5f) col = _blowUpIndicatorColor;
	else if (_timeToExplode < 2 && _timeToExplode > 1.75f) col = _blowUpIndicatorColor;
	else if (_timeToExplode < 1.5f && _timeToExplode > 1.25f) col = _blowUpIndicatorColor;
	else if (_timeToExplode < 1.f && _timeToExplode > 0.75f) col = _blowUpIndicatorColor;
	else if (_timeToExplode < 0.5 && _timeToExplode > 0.25f) col = _blowUpIndicatorColor;
	
	// Body
	DrawTexturePro
	(
		GameManager::instance->sprBombBody,
		{ float(int(_animationFrame) * BOMB_SPRITE_SIZE), float(int(_type) * BOMB_SPRITE_SIZE), BOMB_SPRITE_SIZE, BOMB_SPRITE_SIZE }, // SOURCE
		{ _position.x, _position.y, _scale * _grabbedScaleMultiplier, _scale * _grabbedScaleMultiplier }, // DEST
		{ _radius * _grabbedScaleMultiplier, _radius * _grabbedScaleMultiplier }, // ORIGIN
		0, // ROTATION
		WHITE // TINT
	);

	// Deco
	DrawTexturePro
	(
		GameManager::instance->sprBombDeco,
		{ float(int(_animationFrame) * BOMB_SPRITE_SIZE), float(_animationIndex * BOMB_SPRITE_SIZE), BOMB_SPRITE_SIZE, BOMB_SPRITE_SIZE }, // SOURCE
		{ _position.x, _position.y, _scale * _grabbedScaleMultiplier, _scale * _grabbedScaleMultiplier }, // DEST
		{ _radius * _grabbedScaleMultiplier, _radius * _grabbedScaleMultiplier }, // ORIGIN
		0, // ROTATION
		WHITE // TINT
	);


	if (DEBUG_BOMB_HITBOX_DRAW)
	{
		DrawCircleLinesV ( _position,
			_radiusVisual, // RADIUS
			col
		);
	}

}

void Bomb::GameOver() 
{
	_didGameOver = true;
}

void Bomb::CheckCollisionWith(Bomb& b)
{
	if (b._state != RANDOM_MOVEMENT || _state != RANDOM_MOVEMENT) return;
	if (b._collidedThisFrame || _collidedThisFrame) return;

	Vector2 delta = Vector2Subtract(_position, b._position);
	float dist = delta.x * delta.x + delta.y * delta.y;
	float minDist = _radiusVisual + b._radiusVisual;

	if (dist < minDist * minDist)
	{
		ResolveCollisionWith(b, delta, sqrt(dist), minDist);
	}
}

void Bomb::ResolveCollisionWith(Bomb& b, Vector2 delta, float dist, float minDist)
{
	if (dist < 0.0001f)
	{
		delta = { 1.0f, 0.0f };
		dist = 1.0f;
	}

	_collidedThisFrame = true;
	b._collidedThisFrame = true;

	Vector2 normal = Vector2Scale(delta, 1.0f / dist);
	float overlap = minDist - dist;

	_position = Vector2Add(_position, Vector2Scale(normal, overlap * 0.5f));
	b._position = Vector2Subtract(b._position, Vector2Scale(normal, overlap * 0.5f));

	_movementDirection.x *= -1;
	b._movementDirection.x *= -1;
	_movementDirection.y *= -1;
	b._movementDirection.y *= -1;
}


bool Bomb::BombLayerSort(const Bomb* a, const Bomb* b)
{
	if (a->_state == GRABBED) return false;
	if (b->_state == GRABBED) return true;

	return a->_position.y < b->_position.y;
}
