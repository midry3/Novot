#include "SoundManager.h"

void _SoundManager::PlayBGM(std::shared_ptr<Sound> bgm) {
	if (this->bgm && this->bgm->IsLoaded()) {
		this->bgm->Release();
	}
	this->bgm = bgm;
	this->bgm->SetLoop(true);
	this->bgm->Play();
}

void _SoundManager::PlayBGM() {
	if (bgm && bgm->IsLoaded()) {
		bgm->SetLoop(true);
		bgm->Play();
	}
}

void _SoundManager::StopBGM() {
	if (bgm && bgm->IsLoaded()) {
		bgm->Pause();
	}
}

void _SoundManager::PlaySE(std::shared_ptr<Sound> sound) {
	sound->SetLoop(false);
	sound->Play();
}

void _SoundManager::PlaySE(const std::string& path, int id) {
	std::shared_ptr<Sound> sound = AssetManager.LoadAsset<Sound>(path, id);
	sound->SetLoop(false);
	sound->Play();
}