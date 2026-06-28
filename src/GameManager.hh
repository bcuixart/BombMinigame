#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include <iostream>
#include <vector> 
#include <algorithm> 
#include <memory> 

#include <raylib.h>
#include <raymath.h>

#include "AudioManager.hh"
#include "GameObject.hh"
#include "Bomb.hh"
#include "BombHouse.hh"
#include "Explosion.hh"
#include "GameOverOverlay.hh"
#include "Constants.hh"

using namespace std;

enum GameState 
{
	MAIN_MENU,
	ROUND,
	GAME_OVER_CUTSCENE,
	GAME_OVER,
};

struct RoundValues
{
	int score;

	vector<BombType> spawnableBombTypes;
	int spawnedBombTypes[BOMB_TYPE_COUNT];

	BombType currentBombHouseTopType;
	BombType currentBombHouseBottomType;

	float roundTimeElapsed;
	
	float timeToSpawnNextBomb;
	float nextBombSpawnTime;

	float currentMaxBombs;

	float timeToChangeBombHouse;
	float nextBombHouseChangeTime;

	float timeForNextDramaticDrum;

	bool addedBlue;
	bool addedGreen;
};

class GameManager {
public:
	GameManager();
	~GameManager();

	void Update(const float deltaTime);
	void Render(const float deltaTime);

	void InstantiateBomb(std::unique_ptr<Bomb> obj);
	void DestroyBomb(Bomb* obj);
	void DestroyExplosion(Explosion* expl);

	void BombEntered(Bomb* obj, int _placedDirection);
	void ExplodeBomb(Bomb* obj);

	bool PlayerExited() const;

	bool IsGameOverCutscene() const;

	Vector2 GetWorldMousePos() const;

	static GameManager* instance;

	std::unique_ptr<AudioManager> audioManager;

	Texture2D sprBombBody;
	Texture2D sprBombDeco;
	Texture2D sprBombFuse;
	Texture2D sprBombHouse;
	Texture2D sprBombHouseScreen;
	Texture2D sprExplosion;
	Texture2D sprExplosionProps;
	Texture2D sprGameOverOverlay;

protected:

private:
	void StartGame();
	void StartMainMenu();

	void UpdateMainMenu(const float deltaTime);
	void UpdateRound(const float deltaTime);
	void UpdateGameOver(const float deltaTime);
	void UpdateGameOverCutscene(const float deltaTime);

	void GameOver(Bomb* obj);

	void HandleBombGrab();
	void TryGrabBomb(const Vector2 mousePos);
	int GetBombReleasedState(Bomb* obj);

	void CheckBombCollisions();

	BombType GetNewBombType() const;
	void ChangeBombHouseTypes();

	void InstantiateExplosion(const Vector2 position, const float pan);

	void AddScreenShake(float amount);

	void DrawScreenNumber(const int score, const Vector2 position) const;

	GameState _state;
	RoundValues _roundValues;

	vector<std::unique_ptr<Bomb>> _bombGameObjects;
	vector<std::unique_ptr<Explosion>> _explosionGameObjects;

	Camera2D _cam = { 0 };

	float _screenShakeTrauma = 0.0f;
	float _screenShakePhase = 0.0f;

	std::unique_ptr<BombHouse> _bombHouseTop;
	std::unique_ptr<BombHouse> _bombHouseBottom;

	std::unique_ptr<GameOverOverlay> _gameOverOverlay;

	bool _currentPressed;
	bool _prevPressed;

	Bomb* _grabbedBomb;
	Bomb* _gameOverBomb;

	float _gameOverCutsceneTimer;
	float _gameOverRestartTimer;

	Texture2D _sprMapBG;
	Texture2D _sprMapMG;
	Texture2D _sprMapFG;
	Texture2D _sprScreenNumbers;

	bool _playerExited;
};

#endif