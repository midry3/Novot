#include "MathUtils.h"

bool IsColliding1D(double a1, double a2, double b1, double b2) {
	return b1 <= a2 && a1 <= b2;
}

double ToDouble(const std::variant<double, float, int>& v) {
	if (std::holds_alternative<double>(v)) {
		return std::get<double>(v);
	}
	else if (std::holds_alternative<float>(v)) {
		return static_cast<double>(std::get<float>(v));
	}
	else if (std::holds_alternative<int>(v)) {
		return static_cast<double>(std::get<int>(v));
	}
	return 0.0;
}