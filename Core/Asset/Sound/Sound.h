#pragma once
#include <Asset/Asset.h>
#include "../Asset.h"

class Sound : public Asset {
protected:
	bool isLoop = false;
public:
	Sound() = default;
	Sound(const fs::path& file) : Asset(file) {}
	~Sound() {
		Release();
	}
	int Load() override;
	void Release() override;
	void Play();
	void Pause();
	void Stop();
	Sound* SetLoop(bool loop) {
		isLoop = loop;
		return this;
	}
};