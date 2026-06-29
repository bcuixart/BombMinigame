#ifndef BOMBHOUSE_HH
#define BOMBHOUSE_HH

#include "GameObject.hh"
#include "Bomb.hh"

using namespace std;

enum BombHouseType {
	BOMBHOUSE_TOP,
	BOMBHOUSE_BOTTOM
};

class BombHouse : public GameObject {
public:
	BombHouse(const Vector2 p, const float r, const float s, BombHouseType h);
	virtual ~BombHouse() {};

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

	virtual void GameOver();

	void RenderScreen() const;

	void SetType(BombType t, bool isFirstTime);
	BombType GetType() const;

	bool GetIsBombEnteredTypeValid(BombType t) const;

protected:

private:
	BombHouseType _houseType;
	BombType _bombType;
	BombType _bombTypeOld;

	float _radius;

	float _lenienceTime;
	bool _isTransitioning;
	bool _isScreenTransitionBright;
	bool _wasScreenTransitionBrightLastFrame;
	float _screenFlashStartTime;

	int _screenSprite;
	int _screenSpriteOld;

	float _animationFrame;

	Color _color;
};

#endif