#pragma once

class Vector3D {
public:
	double x;
	double y;
	double z;
	constexpr Vector3D() : x(0), y(0), z(0) {}
	constexpr Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
	unsigned int ToColor() const;
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