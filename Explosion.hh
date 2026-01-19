#ifndef EXPLOSION_HH
#define EXPLOSION_HH

#include <iostream>
#include <random>

#include "GameObject.hh"

using namespace std;

class Explosion : public GameObject {
public:
	Explosion(const Vector2 p, const float r, const float s);
	virtual ~Explosion() {};

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

protected:

private:
	float _elapsedLifetime;
};

#endif