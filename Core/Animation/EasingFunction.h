#pragma once
#include <functional>
#include <string>
#include <map>

namespace Easing {
	using EasingFunction = std::function<double(double)>;
	extern std::map<std::string, EasingFunction> EasingFunctionMap;
	EasingFunction GetEasingFunction(const std::string& name);
	bool EasingExists(const std::string& name);
	double Linear(double x);
	double EaseInQuad(double x);
	double EaseOutQuad(double x);
	double EaseInOutQuad(double x);
	double EaseInCubic(double x);
	double EaseOutCubic(double x);
	double EaseInOutCubic(double x);
}