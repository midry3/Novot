#include "Vector2D.h"
#include "Utils.h"

bool IsColliding1D(double a1, double a2, double b1, double b2) {
	return b1 <= a2 && a1 <= b2;
}

Vector2D Sign(Vector2D v) {
	return Vector2D(Sign(v.x), Sign(v.y));
}