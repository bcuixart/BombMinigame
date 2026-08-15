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
#include "Pipe.hh"
#include "Explosion.hh"
#include "GameOverOverlay.hh"
#include "Constants.hh"

using namespace std;

enum GameState 
{
	MAIN_MENU,
	MENU_INFO,
	ROUND,
	ROUND_PAUSED,
	ROUND_RESUMING,
	GAME_OVER_CUTSCENE,
	GAME_OVER,
	REVIVE_DECISION,
	POINT_TALLY,
	POINT_TALLY_DONE
};

enum MenuInfoState { MENU_INFO_NONE = 0, MENU_INFO_NEXT = 1, MENU_INFO_EXIT = 2 };
enum PauseState { PAUSE_NONE = 0, PAUSE_HOLD_RESUME = 1, PAUSE_HOLD_EXIT = 2 };

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

	int musicChangeBombHouseIndex;

	float timeForNextDramaticDrum;

	bool addedBlue;
	bool addedGreen;

	bool canRevive;
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

	bool IsPaused() const;
	bool PlayerExited() const;

	bool IsGameOverCutscene() const;

	Vector2 GetWorldMousePos() const;

	Vector2 GetHorizontalBounds() const;

	float GetPan(const Vector2& position) const;
	float GetBombExplosionSoundVolume() const;

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
	Texture2D sprPipeValve;
	Texture2D sprPipeValveIndicator;
	Texture2D sprSteamCloud;
	Texture2D sprSteamEmitter;
	Texture2D sprSteamIndicatorBase;
	Texture2D sprSteamIndicatorNeedle;

protected:

private:
	void StartGame();
	void StartGameRevive();
	void StartMainMenu();
	void StartMenuInfo();
	void StartReviveDecision();
	void StartPointTally();

	void UpdateMainMenu(const float deltaTime);
	void UpdateMenuInfo(const float deltaTime);
	void UpdateRound(const float deltaTime);
	void UpdateRoundPaused(const float deltaTime);
	void UpdateRoundResuming(const float deltaTime);
	void UpdateGameOver(const float deltaTime);
	void UpdateGameOverCutscene(const float deltaTime);
	void UpdateReviveDecision(const float deltaTime);
	void UpdatePointTally(const float deltaTime);
	void UpdatePointTallyDone(const float deltaTime);

	void RenderMenuInfo();
	void RenderRoundPaused();
	void RenderRoundResuming();

	bool IsPauseButtonPressed() const;

	void GameOver(Bomb* obj);

	void HandleBombGrab();
	void TryGrabBomb(const Vector2 mousePos);
	int GetBombReleasedState(Bomb* obj);

	void HandlePipeGrab();

	void CheckBombCollisions();

	BombType GetNewBombType() const;
	void ChangeBombHouseTypes();

	float GetBombSpawnPos() const;
	float GetSmallScreenPos() const;
	Rectangle GetPauseButtonRect() const;

	void InstantiateExplosion(const Vector2 position);

	void AddScreenShake(float amount);

	void DrawScreenNumber(const int score, const Vector2 position, const Color color) const;
	void DrawResumeNumber(const int number, const Vector2 position, const Color color) const;

	void GetSaveData();
	void SaveData();

	GameState _state;
	MenuInfoState _menuInfoState;
	int _menuInfoPage;
	PauseState _pauseState;
	RoundValues _roundValues;

	BombType _reviveHouseTopType;
	BombType _reviveHouseBottomType;

	vector<std::unique_ptr<Bomb>> _bombGameObjects;
	vector<std::unique_ptr<Explosion>> _explosionGameObjects;

	Camera2D _cam = { 0 };

	float _screenShakeTrauma = 0.0f;
	float _screenShakePhase = 0.0f;

	float _pauseResumeTimer = 0.0f;
	int _pauseResumeTicks = 0;

	float _musicPrevTime = 0.0f;

	int _highScore;
	float _pointTallyCounter;
	int _pointTallyCounterLast;
	int _pointTallySkipClicks;

	std::unique_ptr<BombHouse> _bombHouseTop;
	std::unique_ptr<BombHouse> _bombHouseBottom;

	std::unique_ptr<Pipe> _pipe;

	std::unique_ptr<GameOverOverlay> _gameOverOverlay;

	bool _currentPressed;
	bool _prevPressed;

	Bomb* _grabbedBomb;
	Bomb* _gameOverBomb;

	Pipe* _grabbedPipe;

	float _gameOverCutsceneTimer;
	float _gameOverRestartTimer;
	float _pointTallyDoneTimer;

	int _pauseDecoVariation;
	bool _pauseButtonHovered;
	bool _pauseButtonHoveredLastFrame;

	Texture2D _sprMapBG;
	Texture2D _sprMapMG;
	Texture2D _sprMapFG;
	Texture2D _sprSmallScreen;
	Texture2D _sprScreenNumbers;
	Texture2D _sprMenuInfo;
	Texture2D _sprPauseMenu;
	Texture2D _sprPauseButton;

	bool _playerExited;
};

#endif