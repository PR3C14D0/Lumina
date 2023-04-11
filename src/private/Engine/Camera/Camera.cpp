#include "Engine/Camera/Camera.h"
#include "Engine/Core.h"

Camera::Camera(std::string name) : GameObject::GameObject(name) {
	Core* core = Core::GetInstance();
	int width, height;
	core->GetWindowSize(width, height);

	this->input = Input::GetInstance();

	this->View = XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(this->transform.location.x, this->transform.location.y, this->transform.location.z));
	this->Projection = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), (float)width / (float)height, 0.001f, 300.f));
}

void Camera::Update() {
	// GameObject::Update(); We'll simply comment this because we want to override our GameObject::Update method.

	this->View = XMMatrixTranspose(XMMatrixIdentity());
	this->View *= XMMatrixTranspose(XMMatrixRotationX(XMConvertToRadians(this->transform.rotation.x)));
	this->View *= XMMatrixTranspose(XMMatrixRotationY(XMConvertToRadians(this->transform.rotation.y)));
	this->View *= XMMatrixTranspose(XMMatrixRotationZ(XMConvertToRadians(this->transform.rotation.z)));
	this->View *= XMMatrixTranspose(XMMatrixTranslation(this->transform.location.x, this->transform.location.y, this->transform.location.z));
}

void Camera::GetMatrices(XMMATRIX& View, XMMATRIX& Projection) {
	View = this->View;
	Projection = this->Projection;

	return;
}