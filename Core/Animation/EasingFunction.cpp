#include "EasingFunction.h"
#include <String/StringUtils.h>

namespace Easing {
	std::map<std::string, EasingFunction> EasingFunctionMap = {
		{"linear", Linear},
		{"easeinquad", EaseInQuad},
		{"easeoutquad", EaseOutQuad},
		{"easeinoutquad", EaseInOutQuad},
		{"easeincubic", EaseInCubic},
		{"easeoutcubic", EaseOutCubic},
		{"easeinoutcubic", EaseInOutCubic}
	};

	EasingFunction GetEasingFunction(const std::string& name) {
		const auto& lname = StringUtils::ToLower(name);
		if (EasingFunctionMap.find(lname) != EasingFunctionMap.end()) {
			return EasingFunctionMap[lname];
		}
		return Linear;
	}

	bool EasingExists(const std::string& name) {
		const auto& lname = StringUtils::ToLower(name);
		return EasingFunctionMap.find(lname) != EasingFunctionMap.end();
	}

	double Linear(double x) {
		return x;
	}

	double EaseInQuad(double x) {
		return x * x;
	}

	double EaseOutQuad(double x) {
		return x * (2 - x);
	}

	double EaseInOutQuad(double x) {
		return x < 0.5 ? 2 * x * x : -1 + (4 - 2 * x) * x;
	}

	double EaseInCubic(double x) {
		return x * x * x;
	}

	double EaseOutCubic(double x) {
		return (--x) * x * x + 1;
	}

	double EaseInOutCubic(double x) {
		return x < 0.5 ? 4 * x * x * x : (x - 1) * (2 * x - 2) * (2 * x - 2) + 1;
	}
}