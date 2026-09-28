#pragma once
#include <cmath>
#include <functional>

bool IsColliding1D(double a1, double a2, double b1, double b2);

class Vector2D {
public:
	double x=0.0, y=0.0;
	constexpr Vector2D() : x(0), y(0) {}
	constexpr Vector2D(double x, double y) : x(x), y(y) {}
	Vector2D operator+(const Vector2D& other) const {
		return Vector2D(x + other.x, y + other.y);
	}
	Vector2D operator-() const {
		return Vector2D(-x, -y);
	}
	Vector2D operator-(const Vector2D& other) const {
		return Vector2D(x - other.x, y - other.y);
	}
	Vector2D operator*(double a) const {
		return Vector2D(x * a, y * a);
	}
	Vector2D operator*(const Vector2D& other) const {
		return Vector2D(x * other.x, y * other.y);
	}
	Vector2D operator/(double a) const {
		return Vector2D(x / a, y / a);
	}
	Vector2D operator/(const Vector2D& other) const {
		return Vector2D(x / other.x, y / other.y);
	}
	Vector2D& operator+=(const Vector2D& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	Vector2D operator-=(const Vector2D& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}
	Vector2D operator*=(double a) {
		x *= a;
		y *= a;
		return *this;
	}
	Vector2D operator*=(const Vector2D& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}
	bool operator==(const Vector2D& other) const {
		return x == other.x && y == other.y;
	}
	bool operator!=(const Vector2D& other) const {
		return !(*this == other);
	}
	double Product(const Vector2D& other) const {
		return x * other.y - y * other.x;
	}
	double Dot(const Vector2D& other) const {
		return x * other.x + y * other.y;
	}
	double Length() const {
		return sqrt(x * x + y * y);
	}
	Vector2D Normalized() const {
		auto length = Length();
		if (length == 0) return Vector2D(0, 0);
		return Vector2D(x / length, y / length);
	}
	Vector2D Normalized(double length) const {
		return Normalized() * length;
	}
	void NormalizeTo(double length) {
		double abs = Length();
		if (abs != 0) {
			x /= abs;
			y /= abs;
			x *= length;
			y *= length;
		}
	}
	Vector2D Perpendicular() const {
		return Vector2D(-y, x);
	}
	Vector2D Perpendicular(const Vector2D& line) const {
		return *this - line * (this->Dot(line) / line.Dot(line));
	}
	Vector2D Sign() const {
		int sx, sy;
		if (x > 0) sx = 1;
		else if (x < 0) sx = -1;
		else sx = 0;
		if (y > 0) sy = 1;
		else if (y < 0) sy = -1;
		else sy = 0;
		return Vector2D(sx, sy);
	}
	Vector2D Abs() const {
		return Vector2D(std::abs(x), std::abs(y));
	}
};

Vector2D Sign(Vector2D v);

constexpr Vector2D Vector0(0.0, 0.0);
constexpr Vector2D Vector1(1.0, 1.0);
constexpr Vector2D VectorUp(0.0, -1.0);
constexpr Vector2D VectorDown(0.0, 1.0);
constexpr Vector2D VectorLeft(-1.0, 0.0);
constexpr Vector2D VectorRight(1.0, 0.0);

class Vector2DFunction : public Vector2D {
protected:
	std::function<double(double)> funcX;
	std::function<double(double)> funcY;
	bool _sumMode = false;
public:
	Vector2DFunction() = default;
	Vector2DFunction(std::function<double(double)> funcX, std::function<double(double)> funcY, bool sumMode = false) : funcX(funcX), funcY(funcY), _sumMode(sumMode) {}
	Vector2D operator()(double t) {
		Update(t);
		return Vector2D(x, y);
	}
	void Update(double t) {
		if (_sumMode) {
			x += funcX(t);
			y += funcY(t);
		}
		else {
			x = funcX(t);
			y = funcY(t);
		}
	}
};