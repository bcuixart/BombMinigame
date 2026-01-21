#include "GameObject.hh"

GameObject::GameObject(const Vector2 p, const float r, const float s)
{
	_position = p;
	_rotation = r;
	_scale = s;

	_markedForDestroy = false;
}

void GameObject::Update(float deltaTime)
{

}

void GameObject::Render(const float deltaTime) 
{

}

void GameObject::GameOver() 
{

}

Vector2 GameObject::GetPosition() const 
{
	return _position;
}

float GameObject::GetRotation() const 
{
	return _rotation;
}

float GameObject::GetScale() const
{
	return _scale;
}

void GameObject::MarkForDestroy()
{
	_markedForDestroy = true;
}

bool GameObject::isMarkedForDestroy() const
{
	return _markedForDestroy;
}