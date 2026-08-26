#pragma once
#include <vector>
#include <functional>

class StateMachine {
protected:
	std::vector<std::pair<std::function<void()>, std::function<void()>>> _stateCallbacks;
	int currentState = 0;
public:
	StateMachine() = default;
	StateMachine(int stateNum) {
		_stateCallbacks.resize(stateNum);
	}
	~StateMachine() = default;
	void AddState(int stateId, std::function<void()> onEnter, std::function<void()> onExit) {
		_stateCallbacks[stateId] = { onEnter, onExit };
	}
	void RegisterStateEnterCallback(int stateId, std::function<void()> callback) {
		if (_stateCallbacks.size() <= stateId) {
			_stateCallbacks.resize(stateId + 1);
		}
		_stateCallbacks[stateId].first = callback;
	}
	void RegisterStateExitCallback(int stateId, std::function<void()> callback) {
		if (_stateCallbacks.size() <= stateId) {
			_stateCallbacks.resize(stateId + 1);
		}
		_stateCallbacks[stateId].second = callback;
	}
	void ChangeState(int newState) {
		if (currentState == newState) return;
		if (_stateCallbacks.size() > currentState) {
			_stateCallbacks[currentState].second();
		}
		currentState = newState;
		if (_stateCallbacks.size() > currentState && _stateCallbacks[currentState].first) {
			_stateCallbacks[currentState].first();
		}
	}
};