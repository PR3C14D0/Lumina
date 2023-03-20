#include "Math/Vector3.h"

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;

	return;
}

Vector3::Vector3(Vector3& v) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

Vector3 Vector3::operator+(Vector3& v) {
	return Vector3{ v.x + this->x, v.y + this->y, v.z + this->z };
}

Vector3 Vector3::operator*(float f) {
	return Vector3{ this->x * f, this->y * f, this->z * f };
}