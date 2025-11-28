#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include <iostream>
#include <vector> 

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"
#include "Bomb.hh"
#include "BombHouse.hh"

using namespace std;

class GameManager {
public:
	GameManager();

	void Update(const float deltaTime);
	void Render(const float deltaTime);

	void GameOver();

	void InstantiateObject(GameObject* obj);
	void DestroyObject(GameObject* obj);

	int GetBombReleasedState(Bomb* obj);

	static GameManager* instance;

protected:

private:
	void StartGame();

	vector<GameObject*> _gameObjects;

	const float BOMB_SPAWN_TIME_START = 3;
	float bombSpawnTime = 3;

	float timeToSpawnNextBomb = 0;

	BombHouse* _bombHouseLeft;
	BombHouse* _bombHouseRight;

	bool didGameOver;
};

#endif