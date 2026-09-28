#pragma once
#include "Vector2D.h"

class LineSegment {
protected:
	Vector2D _start, _end;
public:
	LineSegment() : _start(Vector2D(0, 0)), _end(Vector2D(0, 0)) {}
	LineSegment(const Vector2D& start, const Vector2D& end) : _start(start), _end(end) {}
	~LineSegment() = default;

	bool IsCrossing(const LineSegment& other) const;
	Vector2D GetCrossingPoint(const LineSegment& other) const;
	Vector2D NormalVector() const;
};