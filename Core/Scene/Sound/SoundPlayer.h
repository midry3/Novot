#pragma once
#include <Scene.h>
#include <Asset/Sound/Sound.h>

class SoundPlayer : public Scene {
protected:
	std::shared_ptr<Sound> _sound;

	void _Load() override;
public:
	SoundPlayer* SetSound(const std::shared_ptr<Sound>& sound);
	void Play();
	void Pause();
	void Stop();
};