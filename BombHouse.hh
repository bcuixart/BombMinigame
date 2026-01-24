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

	Color _color;
};

#endif