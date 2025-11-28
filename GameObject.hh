#ifndef GAMEOBJECT_HH
#define GAMEOBJECT_HH

#include <iostream>

#include <raylib.h>
#include <raymath.h>

using namespace std;

class GameObject {
public:
	GameObject(const Vector2 p, const float r, const float s);

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

protected:
	Vector2 _position;
	float _rotation;
	float _scale;

private:

};

#endif