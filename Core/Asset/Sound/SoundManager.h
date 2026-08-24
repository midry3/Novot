#pragma once
#include "../AssetLoader.h"
#include <Singleton.h>
#include "Sound.h"
#include <memory>

class _SoundManager : public Singleton<_SoundManager> {
protected:
	std::shared_ptr<Sound> bgm;
	bool muted = false;
public:
	void PlayBGM(std::shared_ptr<Sound> bgm);
	void PlayBGM();
	void StopBGM();
	void PlaySE(std::shared_ptr<Sound> sound);
	void PlaySE(const std::string& path, int id);

	// Introspection for debugger
	std::shared_ptr<Sound> GetBGM() const { return bgm; }
	void MuteAll() { muted = true; /* implementation-specific: stop/mute audio system */ }
	void UnmuteAll() { muted = false; }
	bool IsMuted() const { return muted; }
};

#define SoundManager _SoundManager::GetInstance()