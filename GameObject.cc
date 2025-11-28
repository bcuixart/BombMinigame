#include "GameObject.hh"

GameObject::GameObject(const Vector2 p, const float r, const float s)
{
	_position = p;
	_rotation = r;
	_scale = s;
}

void GameObject::Update(float deltaTime)
{

}

void GameObject::Render(const float deltaTime) 
{

}