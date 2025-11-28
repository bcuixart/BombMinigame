#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include <iostream>
#include <vector> 

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"
#include "Bomb.hh"

using namespace std;

class GameManager {
public:
	GameManager();

	void Update(const float deltaTime);
	void Render(const float deltaTime);

	void InstantiateObject(GameObject* obj);
	void DestroyObject(GameObject* obj);

	static GameManager* instance;

protected:

private:
	vector<GameObject*> _gameObjects;

	const float BOMB_SPAWN_TIME = 3;

	float timeToSpawnNextBomb = 0;
};

#endif