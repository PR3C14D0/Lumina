#pragma once

struct Vector3 {
	float x, y, z;

	Vector3(float x, float y, float z);
	Vector3(Vector3& v);
	Vector3() = default;

	Vector3 operator+(Vector3& v);
	Vector3 operator*(float f);
};