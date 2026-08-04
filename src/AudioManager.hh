#ifndef AUDIOMANAGER_HH
#define AUDIOMANAGER_HH

#include <iostream>
#include <string>
#include <raylib.h>
#include <raymath.h>

#include "Constants.hh"

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	void Update(const float deltaTime);

	void PlayMusic();
	void PauseMusic();
	void ResumeMusic();
	void StopMusic();

	float GetMusicTime() const;

	void PlayBombStepSound(const float pan);
	void PlayBombCollisionSound(const float pan);

	void PlayBombGrabbedSound(const float pan);
	void PlayBombReleasedBombHouseSound(const float pan);
	void PlayBombReleasedMetalSound(const float pan);
	void PlayBombWarningSound(const float pan);

	void PlayBombHouseTransitionFlashSound();

	void PlayPipeValveRotateSound();

	void PlayPointSound();
	void PlayPointTallySound();

	void PlayGameOverAlertSound();
	void PlayGameOverJingleSound();

	void PlayDramaticDrum();

	void PlayPointTallyEndSound();
	void PlayPointTallyEndHighScoreSound();

	Sound GetBombWindUpLoopSound();
	void UnloadBombWindUpLoopSound(Sound* sound);

	Sound GetBombFuseLoopSound();
	void UnloadBombFuseLoopSound(Sound* sound);

	Sound GetBombExplosionSound();
	void UnloadBombExplosionSound(Sound* sound);

protected:

private:
	Sound _bombStepSounds[ASSET_SOUND_BOMB_STEP_SOUNDS];
	Sound _bombCollisionSounds[ASSET_SOUND_BOMB_COLLISION_SOUNDS];

	Sound _bombGrabbedSound;
	Sound _bombReleasedBombHouseSound;
	Sound _bombReleasedMetalSound;
	Sound _bombWarningSound;

	Sound _bombHouseTransitionFlashSound;

	Sound _pipeValveRotateSound[ASSET_SOUND_PIPE_VALVE_ROTATE_SOUNDS];
	int _currentPipeValveRotatePointSoundIndex;

	Sound _pointSound[ASSET_SOUND_POINT_SOUNDS];
	int _currentPointSoundIndex;

	Sound _pointTallySound[ASSET_SOUND_POINT_SOUNDS];
	int _currentPointTallySoundIndex;

	Sound _dramaticDrumSound;
	Sound _gameOverAlertSound;
	Sound _gameOverJingleSound;

	Sound _pointTallyEndSound;
	Sound _pointTallyEndHighScoreSound;

	Music _gameMusic;
	bool _playingMusic;
	bool _musicPaused;

	Music _factoryAmbience;

	int _currentBombStepSoundIndex;
	int _currentBombCollisionSoundIndex;
};

#endif