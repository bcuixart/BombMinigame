#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include <iostream>
#include <vector> 

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"

using namespace std;

class GameManager {
public:
	GameManager();

	void Update(const float deltaTime);
	void Render(const float deltaTime);

protected:

private:
	vector<GameObject*> _gameObjects;
};

#endif