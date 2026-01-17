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
	BOMBHOUSE_TOP,
	BOMBHOUSE_BOTTOM
};

class BombHouse : public GameObject {
public:
	BombHouse(const Vector2 p, const float r, const float s, BombHouseType t);

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

	bool GetIsMouseInside(Bomb* obj);

protected:

private:
	
	BombHouseType _houseType;
	BombType _bombType;

	Color _color;
};

#endif