#ifndef BOMB_HH
#define BOMB_HH

#include <iostream>
#include <random>

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"

using namespace std;

class Bomb : public GameObject {
public:
	Bomb(const Vector2 p, const float r, const float s);

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

protected:

private:
	
	const Vector2 BOMB_POSITION_LIMIT_MIN = {150, 0};
	const Vector2 BOMB_POSITION_LIMIT_MAX = {350,500};

	Vector2 _movementSpeed;

	float _timeToChangeDirection;
};

#endif