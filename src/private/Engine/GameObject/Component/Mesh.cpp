#include "Engine/GameObject/Component/Mesh.h"
#include "Engine/Core.h"

Mesh::Mesh() : Component::Component() {
	this->core = Core::GetInstance();
}

void Mesh::Start() {
	Component::Start();
}

void Mesh::Update() {
	Component::Update();
}