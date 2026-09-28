#pragma once
#include <map>
#include <string>
#include <memory>
#include "SpriteSheet.h"

class SpriteSheetManager {
private:
	std::map<std::string, std::shared_ptr<SpriteSheet>> spriteSheets;
	std::string currentAnimation;
	bool isPaused = false;
public:
	Signal<int> FrameChanged;
	Signal<std::string> AnimationFinished;
	SpriteSheetManager() : spriteSheets(), currentAnimation("default") {}
	~SpriteSheetManager() = default;
	void RegisterSpriteSheet(const std::string& animName, std::shared_ptr<SpriteSheet> spriteSheet) {
		spriteSheets[animName] = spriteSheet;
		spriteSheet->FrameChanged.Connect([this](int frame) {
			FrameChanged.Emit(frame);
			});
		spriteSheet->AnimationFinished.Connect([this]() {
			AnimationFinished.Emit(currentAnimation);
			});
		spriteSheet->Load();
	}
	[[nodiscard]] std::shared_ptr<SpriteSheet> GetSpriteSheet(const std::string& animName) const {
		auto it = spriteSheets.find(animName);
		return (it != spriteSheets.end()) ? it->second : nullptr;
	}
	[[nodiscard]] std::shared_ptr <SpriteSheet> GetCurrentSpriteSheet() const {
		return GetSpriteSheet(currentAnimation);
	}
	[[nodiscard]] Vector2D GetCurrentSheetSize() const {
		if (auto sheet = GetCurrentSpriteSheet()) {
			return sheet->GetSheetSize();
		}
		return Vector2D(0, 0);
	}
	[[nodiscard]] Vector2D GetCurrentFrameSize() const {
		if (auto sheet = GetCurrentSpriteSheet()) {
			return sheet->GetFrameSize();
		}
		return Vector2D(0, 0);
	}
	void SetCurrentAnimation(const std::string& animName) {
		if (currentAnimation == animName) return;
		if (auto sheet = GetCurrentSpriteSheet()) {
			sheet->Pause();
			sheet->Init();
		}
		if (spriteSheets.find(animName) != spriteSheets.end()) {
			currentAnimation = animName;
			if (isPaused) {
				spriteSheets[currentAnimation]->Pause();
			}
			else {
				spriteSheets[currentAnimation]->Play();
			}
		}
	}
	void Release() {
		for (auto& pair : spriteSheets) {
			pair.second->Release();
		}
		spriteSheets.clear();
		currentAnimation.clear();
	}
	void Pause() {
		isPaused = true;
		if (auto sheet = GetCurrentSpriteSheet()) sheet->Pause();
	}
	void Play() {
		isPaused = false;
		if (auto sheet = GetCurrentSpriteSheet()) sheet->Play();
	}
	bool IsPaused() const {
		return isPaused;
	}

	[[nodiscard]] std::vector<std::string> GetRegisteredNames() const {
		std::vector<std::string> names;
		names.reserve(spriteSheets.size());
		for (const auto& p : spriteSheets) names.push_back(p.first);
		return names;
	}
	void ReleaseSpriteSheet(const std::string& name) {
		auto it = spriteSheets.find(name);
		if (it != spriteSheets.end()) {
			it->second->Release();
			spriteSheets.erase(it);
		}
	}
	void ReloadSpriteSheet(const std::string& name) {
		auto it = spriteSheets.find(name);
		if (it != spriteSheets.end()) {
			it->second->Release();
			it->second->Load();
		}
	}

	std::shared_ptr<SpriteSheet> operator[](const std::string& animName) {
		return GetSpriteSheet(animName);
	}
};