#include "Engine/Camera/Camera.h"
#include "Engine/Core.h"

Camera::Camera(std::string name) : GameObject::GameObject(name) {
	Core* core = Core::GetInstance();
	int width, height;
	core->GetWindowSize(width, height);

	this->transform.translate(0.f, -1.f, 0.f);
	this->transform.rotate(-45.f, 0.f, 0.f);
	this->View = XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(this->transform.location.x, this->transform.location.y, this->transform.location.z));
	this->Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), (float)width / (float)height, 0.001f, 300.f));
}

void Camera::Update() {
	// GameObject::Update(); We'll simply comment this because we want to override our GameObject::Update method.
	
	this->View = XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(this->transform.location.x, this->transform.location.y, this->transform.location.z));
	this->View *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->transform.rotation.x)));
	this->View *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->transform.rotation.y)));
	this->View *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->transform.rotation.z)));
}

void Camera::GetMatrices(XMMATRIX& View, XMMATRIX& Projection) {
	View = this->View;
	Projection = this->Projection;

	return;
}