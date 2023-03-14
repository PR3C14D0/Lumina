#pragma once
#include "Math/Vector3.h"

class Transform {
public:
	Vector3 location;
	Vector3 rotation;
	Vector3 scale;

	Transform();
};