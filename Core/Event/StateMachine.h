#pragma once
#include <vector>
#include <map>
#include <functional>
#include "Signal.h"

class State {
private:
	std::vector<int> _blackList;

public:
	Signal<> Entered, Exited;

	State& Block(const int state);
	bool CanTransTo(const int state) const;

	void operator-=(const int state) {
		Block(state);
	}
};

class StateMachine {
protected:
	std::map<int, State> _states;
	int currentState = -1;

public:
	StateMachine() = default;

	~StateMachine() = default;
	int GetCurrentState() const {
		return currentState;
	}
	State& AddState(int stateId) {
		return _states[stateId] = State();
	}
	StateMachine RegisterStateEnterCallback(int stateId, std::function<void()> callback) {
		if (_states.size() <= stateId) {
			AddState(stateId);
		}
		_states[stateId].Entered += callback;
		return *this;
	}
	StateMachine RegisterStateExitCallback(int stateId, std::function<void()> callback) {
		if (_states.size() <= stateId) {
			AddState(stateId);
		}
		_states[stateId].Exited += callback;
		return *this;
	}
	void ChangeState(int newState) {
		if (currentState == newState || not _states[currentState].CanTransTo(newState)) return;
		if (currentState < _states.size()) {
			_states[currentState].Exited.Emit();
		}
		currentState = newState;
		if (currentState < _states.size()) {
			_states[currentState].Entered.Emit();
		}
	}
	State& operator[](int stateId) {
		if (_states.size() <= stateId) {
			AddState(stateId);
		}
		return _states[stateId];
	}
	void operator>>(int newState) {
		ChangeState(newState);
	}
	bool operator==(int stateId) const {
		return currentState == stateId;
	}
};