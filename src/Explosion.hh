#ifndef EXPLOSION_HH
#define EXPLOSION_HH

#include "GameObject.hh"

using namespace std;

class Explosion : public GameObject {
public:
	Explosion(const Vector2 p, const float r, const float s);
	virtual ~Explosion();

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

protected:

private:
	float _elapsedLifetime;

	bool _propsEnabled[EXPLOSION_PROPS_NUMBER];
	Vector2 _propsPositions[EXPLOSION_PROPS_NUMBER];
	Vector2 _propsVelocities[EXPLOSION_PROPS_NUMBER];
	float _propsRotations[EXPLOSION_PROPS_NUMBER];
	float _propsRotationSpeeds[EXPLOSION_PROPS_NUMBER];

	Sound _explosionSound;
};

#endif