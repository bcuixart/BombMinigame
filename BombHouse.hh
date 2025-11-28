#ifndef BOMBHOUSE_HH
#define BOMBHOUSE_HH

#include <iostream>
#include <random>

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"
#include "Bomb.hh"

using namespace std;

enum BombHouseType {
	BOMBHOUSE_LEFT,
	BOMBHOUSE_RIGHT
};

class BombHouse : public GameObject {
public:
	BombHouse(const Vector2 p, const float r, const float s, BombHouseType t);

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

	bool GetIsMouseInside(Bomb* obj);

protected:

private:
	
	BombHouseType _type;

	Color _color;
};

#endif