#include "Bomb.hh"
#include "GameManager.hh"

Bomb::Bomb(const Vector2 p, const float r, const float s) : 
	GameObject(p, r, s)
{
	_radius = s / 2;

	_movementDirection = { 1, 1 };

	_type = (rand() % 2 == 0) ? BOMB_BLACK : BOMB_RED;
	_color = (_type == BOMB_BLACK) ? (Color) { 55, 55, 55, 255 } : (Color) { 200, 0, 0, 255 };
	_state = RANDOM_MOVEMENT;

	_timeToExplode = 30;

	_animationFrame = 0;
}

BombType Bomb::GetType() const {
	return _type;
}

void Bomb::Update(float deltaTime)
{
	_animationFrame += ANIMATION_SPEED * deltaTime;
	if (int(_animationFrame) >= ANIMATION_FRAMES) _animationFrame = 0;

	switch (_state) {
		case RANDOM_MOVEMENT: {
			Update_RandomMovement(deltaTime);
			break;
		}
		case GRABBED: {
			Update_Grabbed(deltaTime);
			break;
		}		
		case RETURN_TO_CENTER: {
			Update_ReturnToCenter(deltaTime);
			break;
		}
		case PLACED_LEFT: {
			Update_PlacedLeft(deltaTime);
			break;
		}
		case PLACED_RIGHT: {
			Update_PlacedRight(deltaTime);
			break;
		}
		case GAMEOVER: {
			Update_GameOver(deltaTime);
			break;
		}
	}
}

void Bomb::Update_RandomMovement(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->GameOver();

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

	if (IsMouseButtonPressed(0)) 
	{
		if (CheckCollisionPointCircle(GameManager::instance->GetWorldMousePos(),
			_position,
			_radius * (float(BOMB_SPRITE_HITBOX_SIZE) / float(BOMB_SPRITE_SIZE))))
		{
			_state = GRABBED;
		}
	}
}

void Bomb::Update_Grabbed(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->GameOver();

	_animationIndex = BOMB_ANIM_INDEX_GRABBED;

	_position = GameManager::instance->GetWorldMousePos();

	if (!IsMouseButtonDown(0)) {
		int releasedState = GameManager::instance->GetBombReleasedState(this);

		if (releasedState == -1) {
			GameManager::instance->GameOver();
		} else if (releasedState == 0) {
			_state = RANDOM_MOVEMENT;
		} else if (releasedState == 1) {
			_state = PLACED_LEFT;
		} else if (releasedState == 2) {
			_state = PLACED_RIGHT;
		} else if (releasedState == 3) {
			_state = RANDOM_MOVEMENT;
		}
	}
}

void Bomb::Update_ReturnToCenter(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->GameOver();
}

void Bomb::Update_PlacedLeft(const float deltaTime) 
{
	/*
	_timeToExplode = -1;

	_timeToChangeDirection -= deltaTime;
	if (_timeToChangeDirection <= 0) {
		_timeToChangeDirection = rand() % 5;

		float speedX = (float) ((rand() % 100) - 50);
		float speedY = (float) ((rand() % 100) - 50);

		_movementSpeed = { speedX, speedY };
	}

	_position.x += _movementSpeed.x * deltaTime;
	_position.y += _movementSpeed.y * deltaTime;

	_position = Vector2Clamp(_position, 
		BOMB_POSITION_PLACED_LEFT_LIMIT_MIN, 
		BOMB_POSITION_PLACED_LEFT_LIMIT_MAX);
	*/
}

void Bomb::Update_PlacedRight(const float deltaTime) 
{
	/*
	_timeToExplode = -1;

	_timeToChangeDirection -= deltaTime;
	if (_timeToChangeDirection <= 0) {
		_timeToChangeDirection = rand() % 5;

		float speedX = (float) ((rand() % 100) - 50);
		float speedY = (float) ((rand() % 100) - 50);

		_movementSpeed = { speedX, speedY };
	}

	_position.x += _movementSpeed.x * deltaTime;
	_position.y += _movementSpeed.y * deltaTime;

	_position = Vector2Clamp(_position, 
		BOMB_POSITION_PLACED_RIGHT_LIMIT_MIN, 
		BOMB_POSITION_PLACED_RIGHT_LIMIT_MAX);
	*/
}

void Bomb::Update_GameOver(const float deltaTime)
{
	
}

void Bomb::Render(const float deltaTime) 
{
	Color col = _color;
	if (_timeToExplode < 5 && _timeToExplode > 4.5f) {
		col = _blowUpIndicatorColor;
	}
	else if (_timeToExplode < 4 && _timeToExplode > 3.5f) {
		col = _blowUpIndicatorColor;
	}
	else if (_timeToExplode < 3 && _timeToExplode > 2.5f) {
		col = _blowUpIndicatorColor;
	}
	else if (_timeToExplode < 2 && _timeToExplode > 1.75f) {
		col = _blowUpIndicatorColor;
	}
	else if (_timeToExplode < 1.5f && _timeToExplode > 1.25f) {
		col = _blowUpIndicatorColor;
	}
	else if (_timeToExplode < 1.f && _timeToExplode > 0.75f) {
		col = _blowUpIndicatorColor;
	}
	else if (_timeToExplode < 0.5 && _timeToExplode > 0.25f) {
		col = _blowUpIndicatorColor;
	}

	// Body
	DrawTexturePro(
		GameManager::instance->sprBombBody,
		{ float(int(_animationFrame) * BOMB_SPRITE_SIZE), float(int(_type) * BOMB_SPRITE_SIZE), BOMB_SPRITE_SIZE, BOMB_SPRITE_SIZE }, // SOURCE
		{ _position.x, _position.y, _scale, _scale }, // DEST
		{ _radius, _radius }, // ORIGIN
		0, // ROTATION
		WHITE // TINT
	);

	// Deco
	DrawTexturePro(
		GameManager::instance->sprBombDeco,
		{ float(int(_animationFrame) * BOMB_SPRITE_SIZE), float(_animationIndex * BOMB_SPRITE_SIZE), BOMB_SPRITE_SIZE, BOMB_SPRITE_SIZE }, // SOURCE
		{ _position.x, _position.y, _scale, _scale }, // DEST
		{ _radius, _radius }, // ORIGIN
		0, // ROTATION
		WHITE // TINT
	);


	DrawCircleLinesV(
		_position,
		_radius * (float(BOMB_SPRITE_HITBOX_SIZE) / float(BOMB_SPRITE_SIZE)), // RADIUS
		col
	);
}

void Bomb::GameOver() 
{
	_state = GAMEOVER;
}