#include "AudioManager.hh"

AudioManager::AudioManager()
{
	for (int i = 0; i < ASSET_SOUND_BOMB_STEP_SOUNDS; i++)
	{
		_bombStepSounds[i] = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_STEP_PREFIX + 
		((i < 10) ? "00" : "0") + std::to_string(i) + ASSET_SOUND_BOMB_STEP_SUFFIX).c_str());
	}

	_bombWindUpLoopSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_WINDUP_LOOP).c_str());

	_currentBombStepSoundIndex = 0;
}

AudioManager::~AudioManager()
{
	for (int i = 0; i < ASSET_SOUND_BOMB_STEP_SOUNDS; i++)
	{
		UnloadSound(_bombStepSounds[i]);
	}

	UnloadSound(_bombWindUpLoopSound);
}

void AudioManager::Update(const float deltaTime)
{

}

void AudioManager::PlayBombStepSound(const float pan)
{
	SetSoundPan(_bombStepSounds[_currentBombStepSoundIndex], pan);
	PlaySound(_bombStepSounds[_currentBombStepSoundIndex]);

	_currentBombStepSoundIndex = (_currentBombStepSoundIndex + 1) % ASSET_SOUND_BOMB_STEP_SOUNDS;
}

Sound AudioManager::GetBombWindUpLoopSound()
{
	return LoadSoundAlias(_bombWindUpLoopSound);
}

void AudioManager::UnloadBombWindUpLoopSound(Sound* sound)
{
	UnloadSoundAlias(*sound);
}
