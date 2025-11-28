#include "BombHouse.hh"

BombHouse::BombHouse(const Vector2 p, const float r, const float s, const BombHouseType t) : 
	GameObject(p, r, s)
{
	_type = t;
	_color = (_type == BOMBHOUSE_LEFT) ? RED : BLACK;
}

void BombHouse::Update(float deltaTime)
{

}

void BombHouse::Render(const float deltaTime) 
{
	DrawRectangleV(_position, {100, 100}, _color);
}

bool BombHouse::GetIsMouseInside(Bomb* obj) 
{
	Rectangle rec = {_position.x, _position.y, _position.x+100, _position.y+100};
	return CheckCollisionCircleRec(obj->GetPosition(), obj->GetScale(), rec);
}