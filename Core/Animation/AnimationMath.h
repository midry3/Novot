#pragma once
#include "EasingFunction.h"
#include <Math/Vector2D.h>

double Lerp(double a, double b, double t, Easing::EasingFunction easing = Easing::Linear);
Vector2D Lerp(const Vector2D& a, const Vector2D& b, double t, Easing::EasingFunction easing = Easing::Linear);