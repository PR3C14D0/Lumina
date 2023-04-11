#pragma once
#include "Math/Vector3.h"
#include "DirectXIncludes.h"

class Transform {
public:
	Vector3 location;
	Vector3 rotation;
	Vector3 scale;

	Transform();

	void translate(Vector3 translation);
	void translate(float x, float y, float z);

	void rotate(Vector3 rotation);
	void rotate(float x, float y, float z);

	Vector3 Forward();
	Vector3 Right();
	Vector3 RotatePoint(Vector3 v);
};