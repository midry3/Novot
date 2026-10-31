#pragma once
#include <Singleton.h>
#include <map>
#include <string>
#include <list>

enum DefautKeyMapping {
	KEY_SHIFT
};	

class _KeyMapping : public Singleton<_KeyMapping> {
private:
	std::map<int, std::list<int>> mapping;
	bool _isDetected = false;
	bool _gameInputEnabled = true;
public:
	_KeyMapping() = default;
	~_KeyMapping() = default;
	template <typename... Args>
	requires (std::same_as<Args, int> && ...)
	void RegisterMapping(const int action, Args... keys) {
		for (int key : {keys...}) {
			mapping[action].push_back(key);
		}
	}
	bool IsActionPressing(const int action);
	bool IsActionJustPressed(const int action);
	bool IsActionJustReleased(const int action);
	void SetGameInputEnabled(bool enabled) { _gameInputEnabled = enabled; }
	bool IsGameInputEnabled() const { return _gameInputEnabled; }
	void StartDetect() {
		_isDetected = false;
	}
	bool IsDetected() const {
		return _isDetected;
	}
};

#define KeyMapping _KeyMapping::GetInstance()
