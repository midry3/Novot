#pragma once
#include <Scene.h>
#include <Asset/Sound/Sound.h>

class SoundPlayer : public Scene {
protected:
	static constexpr int NORMAL_VOLUME = 255;

	int _volume = NORMAL_VOLUME;
	std::shared_ptr<Sound> _sound;

	void _Update(float dt) override;
	void _Release() override;
public:
	SoundPlayer* SetSound(const std::shared_ptr<Sound>& sound);
	SoundPlayer* SetVolume(int volume);
	SoundPlayer* SetVolume(float volume);
	SoundPlayer* SetVolume(double volume);
	SoundPlayer* AnimateVolume(int toVolume, float duration);
	SoundPlayer* SetLoop(bool loop=true);
	[[nodiscard]] int GetVolume() const;
	[[nodiscard]] float GetVolumeRatio() const;
	[[nodiscard]] bool IsPlaying() const;
	virtual void Play();
	virtual void Pause();
	virtual void Stop();
};