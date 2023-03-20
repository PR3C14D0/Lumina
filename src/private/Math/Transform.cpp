#include "Math/Transform.h"

Transform::Transform() {
	this->location = Vector3{0.f, 0.f, 0.f};
	this->rotation = Vector3{0.f, 0.f, 0.f};
	this->scale = Vector3{0.f, 0.f, 0.f};
}

void Transform::translate(Vector3 translation) {
	this->location = this->location + translation;
	return;
}

void Transform::translate(float x, float y, float z) {
	Vector3 v = Vector3{ x, y, z };
	this->translate(v);
	return;
}

void Transform::rotate(Vector3 rotation) {
	this->rotation = this->rotation + rotation;
	return;
}

void Transform::rotate(float x, float y, float z) {
	Vector3 v = Vector3{ x, y, z };
	this->rotate(v);
	return;
}