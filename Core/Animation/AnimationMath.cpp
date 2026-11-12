#include "AnimationMath.h"

double Lerp(double a, double b, double t, Easing::EasingFunction easing) {
	return a + (b - a) * easing(t);
}

Vector2D Lerp(const Vector2D& a, const Vector2D& b, double t, Easing::EasingFunction easing) {
	return a + (b - a) * easing(t);
}