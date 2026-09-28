#pragma once
#include "Signal.h"
#include <string>
#include <map>
#include <variant>
#include <vector>
#include <stdexcept>

using DynamicSignalArgumentType = std::variant<int, double, std::string, bool>;

class DynamicSignalArguments {
private:
	std::vector<DynamicSignalArgumentType> _args;
public:
	DynamicSignalArguments() = default;
	DynamicSignalArgumentType operator[](size_t index) const {
		if (index < _args.size()) {
			return _args[index];
		}
		throw std::out_of_range("Index out of range in DynamicSignalArguments");
	}
	size_t size() const {
		return _args.size();
	}

	DynamicSignalArguments* AddArgument(const DynamicSignalArgumentType& arg) {
		_args.push_back(arg);
		return this;
	}
};

class DynamicSignal {
protected:
	std::map<std::string, Signal<DynamicSignalArguments>> _signals;
public:
	Signal<DynamicSignalArguments>& AddSignal(const std::string& name) {
		_signals[name] = Signal<DynamicSignalArguments>();
		return _signals[name];
	}

	Signal<DynamicSignalArguments>& GetSignal(const std::string& name) {
		return _signals[name];
	}

	void Emit(const std::string& name, const DynamicSignalArguments& args) {
		if (_signals.find(name) != _signals.end()) {
			_signals[name].Emit(args);
		}
	}
};