#pragma once
//#include <math.h>
#include <variant>
#include <functional>

bool IsColliding1D(double a1, double a2, double b1, double b2);

double ToDouble(const std::variant<double, float, int>& v);

class Vector2D {
public:
	double x;
	double y;
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
	double Product(const Vector2D& other) const {
		return x * other.y - y * other.x;
	}
	double Dot(const Vector2D& other) const {
		return x * other.x + y * other.y;
	}
	double Abs() const {
		return sqrt(x * x + y * y);
	}
	Vector2D Normalized() const {
		auto length = Abs();
		if (length == 0) return Vector2D(0, 0);
		return Vector2D(x / length, y / length);
	}
	Vector2D Normalized(double length) const {
		return Normalized() * length;
	}
	void NormalizeTo(double length) {
		double abs = Abs();
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
};

constexpr Vector2D Vector0(0.0f, 0.0f);
constexpr Vector2D Vector1(1.0f, 1.0f);
constexpr Vector2D VectorUp(0.0f, -1.0f);
constexpr Vector2D VectorDown(0.0f, 1.0f);
constexpr Vector2D VectorLeft(-1.0f, 0.0f);
constexpr Vector2D VectorRight(1.0f, 0.0f);

class Vector2DFunction : public Vector2D {
protected:
	std::function<double(double)> funcX;
	std::function<double(double)> funcY;
public:
	Vector2DFunction(std::function<double(double)> funcX, std::function<double(double)> funcY) : funcX(funcX), funcY(funcY) {}
	Vector2D operator()(double t) const {
		return Vector2D(funcX(t), funcY(t));
	}
	void Update(double t) {
		x = funcX(t);
		y = funcY(t);
	}
};

class Vector3D {
public:
	double x;
	double y;
	double z;
	constexpr Vector3D() : x(0), y(0), z(0) {}
	constexpr Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
	Vector3D operator+(const Vector3D& other) const {
		return Vector3D(x + other.x, y + other.y, z + other.z);
	}
	Vector3D operator-() const {
		return Vector3D(-x, -y, -z);
	}
	Vector3D operator-(const Vector3D& other) const {
		return Vector3D(x - other.x, y - other.y, z - other.z);
	}
	Vector3D operator*(double a) const {
		return Vector3D(x * a, y * a, z * a);
	}
	Vector3D operator/(double a) const {
		return Vector3D(x / a, y / a, z / a);
	}
};