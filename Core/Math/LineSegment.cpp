#include "LineSegment.h"

bool LineSegment::IsCrossing(const LineSegment& other) const {
	Vector2D p = _start;
	Vector2D r = _end - _start;
	Vector2D q = other._start;
	Vector2D s = other._end - other._start;
	double rxs = r.Product(s);
	double q_pxr = (q - p).Product(r);
	if (rxs == 0 && q_pxr == 0) {
		return IsColliding1D(_start.x, _end.x, other._start.x, other._end.x) &&
			IsColliding1D(_start.y, _end.y, other._start.y, other._end.y);
	}
	if (rxs == 0 && q_pxr != 0) {
		return false;
	}
	double t = (q - p).Product(s) / rxs;
	double u = (q - p).Product(r) / rxs;
	return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

Vector2D LineSegment::GetCrossingPoint(const LineSegment& other) const {
	Vector2D p = _start;
	Vector2D r = _end - _start;
	Vector2D q = other._start;
	Vector2D s = other._end - other._start;
	double rxs = r.Product(s);
	double q_pxr = (q - p).Product(r);
	if (rxs == 0 && q_pxr == 0) {
		return Vector2D((std::max(_start.x, other._start.x) + std::min(_end.x, other._end.x)) / 2,
			(std::max(_start.y, other._start.y) + std::min(_end.y, other._end.y)) / 2);
	}
	if (rxs == 0 && q_pxr != 0) {
		return Vector2D(0, 0);
	}
	double t = (q - p).Product(s) / rxs;
	double u = (q - p).Product(r) / rxs;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
		return p + r * t;
	}
	return Vector0;
}

Vector2D LineSegment::NormalVector() const {
	return (_end - _start).Perpendicular().Normalized();
}