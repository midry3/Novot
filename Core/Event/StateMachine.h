#pragma once
#include <vector>
#include <functional>
#include "Signal.h"

class State {
public:
	Signal<> Entered, Exited;
};

class StateMachine {
protected:
	std::vector<State> _stateCallbacks;
	int currentState = -1;
public:
	StateMachine() = default;
	StateMachine(int stateNum) {
		_stateCallbacks.resize(stateNum);
	}
	~StateMachine() = default;
	int GetCurrentState() const {
		return currentState;
	}
	State& AddState(int stateId) {
		if (_stateCallbacks.size() <= stateId) {
			_stateCallbacks.resize(stateId + 1);
		}
		return _stateCallbacks[stateId];
	}
	StateMachine RegisterStateEnterCallback(int stateId, std::function<void()> callback) {
		if (_stateCallbacks.size() <= stateId) {
			_stateCallbacks.resize(stateId + 1);
		}
		_stateCallbacks[stateId].Entered += callback;
		return *this;
	}
	StateMachine RegisterStateExitCallback(int stateId, std::function<void()> callback) {
		if (_stateCallbacks.size() <= stateId) {
			_stateCallbacks.resize(stateId + 1);
		}
		_stateCallbacks[stateId].Exited += callback;
		return *this;
	}
	void ChangeState(int newState) {
		if (currentState == newState) return;
		if (_stateCallbacks.size() > currentState) {
			_stateCallbacks[currentState].Exited.Emit();
		}
		currentState = newState;
		if (currentState < _stateCallbacks.size()) {
			_stateCallbacks[currentState].Entered.Emit();
		}
	}
	State& operator[](int stateId) {
		if (_stateCallbacks.size() <= stateId) {
			_stateCallbacks.resize(stateId + 1);
		}
		return _stateCallbacks[stateId];
	}
	void operator>>(int newState) {
		ChangeState(newState);
	}
};