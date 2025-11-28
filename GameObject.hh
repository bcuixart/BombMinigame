#ifndef GAMEOBJECT_HH
#define GAMEOBJECT_HH

#include <iostream>

#include <raylib.h>
#include <raymath.h>

using namespace std;

class GameObject {
public:
	GameObject();

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

protected:

private:
	Vector2 _position;
	float _rotation;
	Vector2 _scale;
};

#endif