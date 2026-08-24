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
	SpriteSheetManager() : spriteSheets(), currentAnimation("default") {}
	~SpriteSheetManager() = default;
	void RegisterSpriteSheet(const std::string& animName, std::shared_ptr<SpriteSheet> spriteSheet) {
		spriteSheets[animName] = spriteSheet;
		spriteSheet->Load();
	}
	std::shared_ptr<SpriteSheet> GetSpriteSheet(const std::string& animName) {
		auto it = spriteSheets.find(animName);
		return (it != spriteSheets.end()) ? it->second : nullptr;
	}
	std::shared_ptr <SpriteSheet> GetCurrentSpriteSheet() {
		return GetSpriteSheet(currentAnimation);
	}
	void SetCurrentAnimation(const std::string& animName) {
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
		spriteSheets[currentAnimation]->Pause();
	}
	void Play() {
		isPaused = false;
		spriteSheets[currentAnimation]->Play();
	}
	bool IsPaused() const {
		return isPaused;
	}

	// Debugger introspection
	std::vector<std::string> GetRegisteredNames() const {
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
};