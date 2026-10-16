#pragma once
#include <map>
#include <string>
#include <memory>
#include "SpriteSheet.h"

class SpriteSheetManager {
private:
	Vector<std::shared_ptr<SpriteSheet>> _spriteSheets;
	struct SpriteSheetConnections {
		int frameChanged = -1;
		int animationFinished = -1;
	};
	Vector<SpriteSheetConnections> _connections;
	int _currentAnimation = 0;
	bool isPaused = false;
public:
	Signal<int> FrameChanged, AnimationFinished;
	SpriteSheetManager() : _spriteSheets() {}
	~SpriteSheetManager() = default;
	void RegisterSpriteSheet(const int index, SharedPtr<SpriteSheet> spriteSheet) {
		if (index < 0 || not spriteSheet) {
			return;
		}
		if (_spriteSheets.size() <= index) {
			_spriteSheets.resize(index + 1);
			_connections.resize(index + 1);
		}
		else if (_spriteSheets[index]) {
			_spriteSheets[index]->FrameChanged.Disconnect(_connections[index].frameChanged);
			_spriteSheets[index]->AnimationFinished.Disconnect(_connections[index].animationFinished);
		}
		_spriteSheets[index] = spriteSheet;
		_connections[index].frameChanged = spriteSheet->FrameChanged.Connect([this](int frame) {
			FrameChanged.Emit(frame);
			});
		_connections[index].animationFinished = spriteSheet->AnimationFinished.Connect([this]() {
			AnimationFinished.Emit(_currentAnimation);
			});
		spriteSheet->Load();
	}
	[[nodiscard]] SharedPtr<SpriteSheet> GetSpriteSheet(const int index) const {
		if (index >= 0 && index < _spriteSheets.size()) {
			return _spriteSheets[index];
		}
		return nullptr;
	}
	[[nodiscard]] SharedPtr<SpriteSheet> GetCurrentSpriteSheet() const {
		return GetSpriteSheet(_currentAnimation);
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
	[[nodiscard]] int GetAnimationCount() const {
		return static_cast<int>(_spriteSheets.size());
	}
	[[nodiscard]] int GetCurrentAnimationIndex() const {
		return _currentAnimation;
	}
	void SetCurrentAnimation(const int index) {
		if (_currentAnimation == index) return;
		if (auto sheet = GetCurrentSpriteSheet()) {
			sheet->Pause();
			sheet->Init();
		}
		if (index >= 0 && index < _spriteSheets.size()) {
			_currentAnimation = index;
			if (isPaused) {
				_spriteSheets[_currentAnimation]->Pause();
			}
			else {
				_spriteSheets[_currentAnimation]->Play();
			}
		}
	}
	void Release() {
		for (size_t i = 0; i < _spriteSheets.size(); ++i) {
			if (_spriteSheets[i]) {
				_spriteSheets[i]->FrameChanged.Disconnect(_connections[i].frameChanged);
				_spriteSheets[i]->AnimationFinished.Disconnect(_connections[i].animationFinished);
				_spriteSheets[i]->Release();
			}
		}
		_spriteSheets.clear();
		_connections.clear();
		_currentAnimation = 0;
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
	void ReleaseSpriteSheet(const int index) {
		if (index >= 0 && index < _spriteSheets.size()) {
			if (_spriteSheets[index]) {
				_spriteSheets[index]->FrameChanged.Disconnect(_connections[index].frameChanged);
				_spriteSheets[index]->AnimationFinished.Disconnect(_connections[index].animationFinished);
				_spriteSheets[index]->Release();
			}
			_spriteSheets.erase(_spriteSheets.begin() + index);
			_connections.erase(_connections.begin() + index);
			if (_currentAnimation >= _spriteSheets.size()) {
				_currentAnimation = 0;
			}
		}
	}
	void ReloadSpriteSheet(const int index) {
		if (index >= 0 && index < _spriteSheets.size()) {
			_spriteSheets[index]->Release();
			_spriteSheets[index]->Load();
		}
	}

	SharedPtr<SpriteSheet> operator[](const int index) {
		return GetSpriteSheet(index);
	}
};