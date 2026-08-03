#include "AudioManager.hh"

AudioManager::AudioManager()
{
	for (int i = 0; i < ASSET_SOUND_BOMB_STEP_SOUNDS; i++)
	{
		_bombStepSounds[i] = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_STEP_PREFIX + 
		((i < 10) ? "00" : "0") + std::to_string(i) + ASSET_SOUND_BOMB_STEP_SUFFIX).c_str());
	}
	
	for (int i = 0; i < ASSET_SOUND_BOMB_COLLISION_SOUNDS; i++)
	{
		_bombCollisionSounds[i] = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_COLLISION_PREFIX +
		((i < 10) ? "00" : "0") + std::to_string(i) + ASSET_SOUND_BOMB_COLLISION_SUFFIX).c_str());
	}

	_dramaticDrumSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_DRAMATIC_DRUM).c_str());
	_bombGrabbedSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_GRABBED).c_str());
	_bombReleasedBombHouseSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_RELEASED_BOMB_HOUSE).c_str());
	_bombReleasedMetalSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_RELEASED_METAL).c_str());
	_bombWarningSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_WARNING).c_str());
	_bombHouseTransitionFlashSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMBHOUSE_TRANSITION_FLASH).c_str());
	for (int i = 0; i < ASSET_SOUND_POINT_SOUNDS; i++) _pointSound[i] = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_POINT).c_str());
	for (int i = 0; i < ASSET_SOUND_POINT_SOUNDS; i++) _pointTallySound[i] = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_POINT_TALLY).c_str());
	_gameOverAlertSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_GAMEOVER_ALERT).c_str());
	_gameOverJingleSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_GAMEOVER_JINGLE).c_str());
	_pointTallyEndSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_POINT_TALLY_END).c_str());
	_pointTallyEndHighScoreSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_POINT_TALLY_END_HIGHSCORE).c_str());

	_gameMusic = LoadMusicStream((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_MUSIC).c_str());
	_factoryAmbience = LoadMusicStream((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_FACTORY_AMBIENCE).c_str());

	_currentBombStepSoundIndex = 0;
	_currentBombCollisionSoundIndex = 0;
	_currentPointSoundIndex = 0;
	_currentPointTallySoundIndex = 0;

	_playingMusic = false;
	_musicPaused = false;

	PlayMusicStream(_factoryAmbience);
}

AudioManager::~AudioManager()
{
	for (int i = 0; i < ASSET_SOUND_BOMB_STEP_SOUNDS; i++) UnloadSound(_bombStepSounds[i]);
	for (int i = 0; i < ASSET_SOUND_BOMB_COLLISION_SOUNDS; i++) UnloadSound(_bombCollisionSounds[i]);

	UnloadSound(_dramaticDrumSound);
	UnloadSound(_bombGrabbedSound);
	UnloadSound(_bombReleasedBombHouseSound);
	UnloadSound(_bombReleasedMetalSound);
	UnloadSound(_bombWarningSound);
	UnloadSound(_bombHouseTransitionFlashSound);
	for (int i = 0; i < ASSET_SOUND_POINT_SOUNDS; i++) UnloadSound(_pointSound[i]);
	for (int i = 0; i < ASSET_SOUND_POINT_SOUNDS; i++) UnloadSound(_pointTallySound[i]);
	UnloadSound(_gameOverAlertSound);
	UnloadSound(_gameOverJingleSound);
	UnloadSound(_pointTallyEndSound);
	UnloadSound(_pointTallyEndHighScoreSound);
	UnloadMusicStream(_gameMusic);
	UnloadMusicStream(_factoryAmbience);
}

void AudioManager::Update(const float deltaTime)
{	
	if (_playingMusic || _musicPaused)
	{
		UpdateMusicStream(_gameMusic);

		if (_playingMusic)
		{
			float musicTime = GetMusicTimePlayed(_gameMusic);
			if (musicTime >= ASSET_SOUND_MUSIC_LOOP_END + 1)
			{
				SeekMusicStream(_gameMusic, musicTime - ASSET_SOUND_MUSIC_LOOP_LENGTH);
			}
		}
	}


	UpdateMusicStream(_factoryAmbience);

	float factoryAmbienceTime = GetMusicTimePlayed(_factoryAmbience);
	if (factoryAmbienceTime >= ASSET_SOUND_FACTORY_AMBIENCE_LOOP_END + 1)
	{
		SeekMusicStream(_factoryAmbience, factoryAmbienceTime - ASSET_SOUND_FACTORY_AMBIENCE_LOOP_LENGTH);
	}
}

void AudioManager::PlayMusic()
{
	SeekMusicStream(_gameMusic, 0.1f);
	PlayMusicStream(_gameMusic);
	_playingMusic = true;
	_musicPaused = false;
}

void AudioManager::PauseMusic()
{
	PauseMusicStream(_gameMusic);
	_playingMusic = false;
	_musicPaused = true;
}

void AudioManager::ResumeMusic()
{
	ResumeMusicStream(_gameMusic);
	_playingMusic = true;
	_musicPaused = false;
}

void AudioManager::StopMusic()
{
	StopMusicStream(_gameMusic);
	_playingMusic = false;
	_musicPaused = false;
}

float AudioManager::GetMusicTime() const
{
	if (_playingMusic) return GetMusicTimePlayed(_gameMusic);

	return GetMusicTimePlayed(_factoryAmbience);
}

void AudioManager::PlayBombStepSound(const float pan)
{
	SetSoundPan(_bombStepSounds[_currentBombStepSoundIndex], pan);
	PlaySound(_bombStepSounds[_currentBombStepSoundIndex]);

	_currentBombStepSoundIndex = (_currentBombStepSoundIndex + 1) % ASSET_SOUND_BOMB_STEP_SOUNDS;
}

void AudioManager::PlayBombCollisionSound(const float pan)
{
	SetSoundPan(_bombCollisionSounds[_currentBombCollisionSoundIndex], pan);
	PlaySound(_bombCollisionSounds[_currentBombCollisionSoundIndex]);

	_currentBombCollisionSoundIndex = (_currentBombCollisionSoundIndex + 1) % ASSET_SOUND_BOMB_COLLISION_SOUNDS;
}

void AudioManager::PlayBombHouseTransitionFlashSound()
{
	PlaySound(_bombHouseTransitionFlashSound);
}

void AudioManager::PlayPointSound()
{
	SetSoundPitch(_pointSound[_currentPointSoundIndex], float(GetRandomValue(90, 110)) / 100.0f);
	PlaySound(_pointSound[_currentPointSoundIndex]);

	_currentPointSoundIndex = (_currentPointSoundIndex + 1) % ASSET_SOUND_POINT_SOUNDS;
}

void AudioManager::PlayPointTallySound()
{
	SetSoundPitch(_pointTallySound[_currentPointTallySoundIndex], float(GetRandomValue(90, 110)) / 100.0f);
	PlaySound(_pointTallySound[_currentPointTallySoundIndex]);

	_currentPointTallySoundIndex = (_currentPointTallySoundIndex + 1) % ASSET_SOUND_POINT_SOUNDS;
}

void AudioManager::PlayDramaticDrum()
{
	PlaySound(_dramaticDrumSound);
}
	
void AudioManager::PlayBombGrabbedSound(const float pan)
{
	SetSoundPan(_bombGrabbedSound, pan);
	PlaySound(_bombGrabbedSound);
}

void AudioManager::PlayBombReleasedBombHouseSound(const float pan)
{
	SetSoundPan(_bombReleasedBombHouseSound, pan);
	PlaySound(_bombReleasedBombHouseSound);
}

void AudioManager::PlayBombReleasedMetalSound(const float pan)
{
	SetSoundPan(_bombReleasedMetalSound, pan);
	PlaySound(_bombReleasedMetalSound);
}

void AudioManager::PlayBombWarningSound(const float pan)
{
	SetSoundPan(_bombWarningSound, pan);
	PlaySound(_bombWarningSound);
}

void AudioManager::PlayGameOverAlertSound()
{
	PlaySound(_gameOverAlertSound);
}

void AudioManager::PlayGameOverJingleSound()
{
	PlaySound(_gameOverJingleSound);
}

void AudioManager::PlayPointTallyEndSound()
{
	PlaySound(_pointTallyEndSound);
}

void AudioManager::PlayPointTallyEndHighScoreSound()
{
	PlaySound(_pointTallyEndHighScoreSound);
}

Sound AudioManager::GetBombWindUpLoopSound()
{
	return LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_WINDUP_LOOP).c_str());
}

void AudioManager::UnloadBombWindUpLoopSound(Sound* sound)
{
	UnloadSound(*sound);
}

Sound AudioManager::GetBombFuseLoopSound()
{
	return LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_FUSE_LOOP).c_str());
}

void AudioManager::UnloadBombFuseLoopSound(Sound* sound)
{
	UnloadSound(*sound);
}

Sound AudioManager::GetBombExplosionSound()
{
	return LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_EXPLOSION).c_str());
}

void AudioManager::UnloadBombExplosionSound(Sound* sound)
{
	UnloadSound(*sound);
}
