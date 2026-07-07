#ifndef BOMB_HH
#define BOMB_HH

#include "GameObject.hh"

#define BOMB_POSITION_PLACED_LEFT_LIMIT_MIN { 0, 200 }
#define BOMB_POSITION_PLACED_LEFT_LIMIT_MAX { 100, 300 }
#define BOMB_POSITION_PLACED_RIGHT_LIMIT_MIN { 400, 200 }
#define BOMB_POSITION_PLACED_RIGHT_LIMIT_MAX { 500, 300 }

#define BOMB_MOVEMENT_SPEED 100

#define ANIMATION_FRAMES 30
#define ANIMATION_SPEED 33.75f

using namespace std;

enum BombState 
{
	RANDOM_MOVEMENT,
	GRABBED,
	PLACED,
};

class Bomb : public GameObject {
public:
	Bomb(const Vector2 p, const float r, const float s, const BombType t);
	virtual ~Bomb();

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

	virtual void GameOver();

	bool WasClicked(const Vector2 mousePos) const;

	void Grab();
	int LetGo(int releasedState);
	void MoveToBombHouseMin();

	void CheckCollisionWith(Bomb& b);
	void ResolveCollisionWith(Bomb& b, Vector2 delta, float dist, float minDist);

	static bool BombLayerSort(const Bomb* a, const Bomb* b);

	BombType GetType() const;

protected:

private:
	
	void Update_RandomMovement(const float deltaTime);
	void Update_Grabbed(const float deltaTime);
	void Update_Placed(const float deltaTime);

	bool GetIsAboutToExplode() const;

	BombType _type;
	BombState _state;

	float _radius;
	float _radiusVisual;
	float _grabbedScaleMultiplier;

	Color _color;
	const Color _blowUpIndicatorColor = {200,200,200,255};
	float _alpha;

	Vector2 _movementDirection;

	float _timeToExplode;

	float _animationTime;
	float _animationFrame;
	int _animationIndex;

	bool _collidedThisFrame;

	bool _wasAboutToExplodeLastFrame;

	bool _didGameOver;

	int _placedDirection;

	bool _didStepSound001;
	bool _didStepSound002;

	Sound _windUpLoopSound;
	Sound _fuseLoopSound;
};

#endif