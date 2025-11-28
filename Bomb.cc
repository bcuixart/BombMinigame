#include "Bomb.hh"
#include "GameManager.hh"

Bomb::Bomb(const Vector2 p, const float r, const float s) : 
	GameObject(p, r, s)
{
	_timeToChangeDirection = 0; // Forces instant speed change

	_type = (rand() % 2 == 0) ? BOMB_LEFT : BOMB_RIGHT;
	_color = (_type == BOMB_LEFT) ? (Color){200,0,0,255} : (Color){55,55,55,255};
	_state = RANDOM_MOVEMENT;

	_timeToExplode = 10;
}

BombType Bomb::GetType() const {
	return _type;
}

void Bomb::Update(float deltaTime)
{
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

	_timeToChangeDirection -= deltaTime;
	if (_timeToChangeDirection <= 0) {
		_timeToChangeDirection = rand() % 5;

		float speedX = (float) ((rand() % 200) - 100);
		float speedY = (float) ((rand() % 200) - 100);

		_movementSpeed = { speedX, speedY };
	}

	_position.x += _movementSpeed.x * deltaTime;
	_position.y += _movementSpeed.y * deltaTime;

	_position = Vector2Clamp(_position, 
		BOMB_POSITION_RANDOM_MOVEMENT_LIMIT_MIN, 
		BOMB_POSITION_RANDOM_MOVEMENT_LIMIT_MAX);

	if (IsMouseButtonPressed(0)) {
		if (CheckCollisionPointCircle(GetMousePosition(), _position, _scale)) {
			_state = GRABBED;
		}
	}
}

void Bomb::Update_Grabbed(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->GameOver();

	_position = GetMousePosition();

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
}

void Bomb::Update_PlacedRight(const float deltaTime) 
{
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
	DrawCircleV(_position, _scale, col);
}

void Bomb::GameOver() 
{
	_state = GAMEOVER;
}