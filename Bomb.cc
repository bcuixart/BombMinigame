#include "Bomb.hh"

Bomb::Bomb(const Vector2 p, const float r, const float s) : 
	GameObject(p, r, s)
{
	_timeToChangeDirection = 0; // Forces instant speed change
}

void Bomb::Update(float deltaTime)
{
	_timeToChangeDirection -= deltaTime;
	if (_timeToChangeDirection <= 0) {
		_timeToChangeDirection = 5;

		float speedX = (float) ((rand() % 200) - 100);
		float speedY = (float) ((rand() % 200) - 100);

		std::cout << speedX << ' ' << speedY << std::endl;
		_movementSpeed = { speedX, speedY };
	}

	_position.x += _movementSpeed.x * deltaTime;
	_position.y += _movementSpeed.y * deltaTime;

	_position = Vector2Clamp(_position, BOMB_POSITION_LIMIT_MIN, BOMB_POSITION_LIMIT_MAX);
}

void Bomb::Render(const float deltaTime) 
{
	DrawCircleLinesV(_position, _scale, GREEN);
}