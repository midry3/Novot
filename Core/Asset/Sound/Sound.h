#pragma once
#include <Asset/Asset.h>
#include "../Asset.h"

class Sound : public Asset {
protected:
	bool isLoop = false;
	int volume = 255;
public:
	Sound() = default;
	Sound(const fs::path& file) : Asset(file), volume(255) {}
	~Sound() {
		Release();
	}
	int Load() override;
	void Release() override;
	void Play();
	void Pause();
	Sound* SetLoop(bool loop) {
		isLoop = loop;
		return this;
	}
	Sound* SetVolume(int volume) {
		this->volume = volume;
		return this;
	}
	Sound* SetVolume(float volume) {
		this->volume = static_cast<int>(volume * 255);
		return this;
	}
};