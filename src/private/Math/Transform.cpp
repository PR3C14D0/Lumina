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

Vector3 Transform::Forward() {
	Vector3 v = Vector3{ 0.f, 0.f, -1.f };
	return this->RotatePoint(v);
}

Vector3 Transform::Right() {
	Vector3 v = Vector3{ -1.f, 0.f, 0.f };
	return this->RotatePoint(v);
}

Vector3 Transform::RotatePoint(Vector3 v) {
	XMMATRIX rot = XMMatrixTranspose(XMMatrixIdentity());
	rot *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->rotation.x)));
	rot *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(this->rotation.y)));
	rot *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(this->rotation.z)));

	XMVECTOR rotToPoint = XMVector4Transform(XMVectorSet(v.x, v.y, v.z, 1.f), rot);
	XMFLOAT4 rotToPointFloat;
	XMStoreFloat4(&rotToPointFloat, rotToPoint);

	return Vector3(rotToPointFloat.x, rotToPointFloat.y, rotToPointFloat.z);
}