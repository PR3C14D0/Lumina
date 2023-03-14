#include "Math/Transform.h"

Transform::Transform() {
	this->location = Vector3{0.f, 0.f, 0.f};
	this->rotation = Vector3{0.f, 0.f, 0.f};
	this->scale = Vector3{0.f, 0.f, 0.f};
}