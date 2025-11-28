#ifndef BOMB_HH
#define BOMB_HH

#include <iostream>
#include <random>

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"

using namespace std;

enum BombType {
	BOMB_LEFT,
	BOMB_RIGHT
};

enum BombState {
  RANDOM_MOVEMENT,
  GRABBED,
  RETURN_TO_CENTER,
  PLACED_LEFT,
  PLACED_RIGHT,
  GAMEOVER
};

class Bomb : public GameObject {
public:
	Bomb(const Vector2 p, const float r, const float s);

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

	virtual void GameOver();

	BombType GetType() const;

protected:

private:
	
	void Update_RandomMovement(const float deltaTime);
	void Update_Grabbed(const float deltaTime);
	void Update_ReturnToCenter(const float deltaTime);
	void Update_PlacedLeft(const float deltaTime);
	void Update_PlacedRight(const float deltaTime);
	void Update_GameOver(const float deltaTime);

	BombType _type;
	BombState _state;

	Color _color;
	const Color _blowUpIndicatorColor = {200,200,200,255};

	const Vector2 BOMB_POSITION_RANDOM_MOVEMENT_LIMIT_MIN = {150, 0};
	const Vector2 BOMB_POSITION_RANDOM_MOVEMENT_LIMIT_MAX = {350, 500};	

	const Vector2 BOMB_POSITION_PLACED_LEFT_LIMIT_MIN = {0, 200};
	const Vector2 BOMB_POSITION_PLACED_LEFT_LIMIT_MAX = {100, 300};	
	const Vector2 BOMB_POSITION_PLACED_RIGHT_LIMIT_MIN = {400, 200};
	const Vector2 BOMB_POSITION_PLACED_RIGHT_LIMIT_MAX = {500, 300};

	Vector2 _movementSpeed;

	float _timeToChangeDirection;

	float _timeToExplode;

	Texture2D _texture = LoadTexture("Assets/Sprites/Bomb.png");
};

#endif