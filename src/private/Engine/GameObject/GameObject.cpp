#include "Engine/GameObject/GameObject.h"

GameObject::GameObject(std::string name) {
	this->name = name;
	this->transform = Transform{};
	this->input = Input::GetInstance();
	this->time = Time::GetInstance();
}

void GameObject::Start() {
	for (Component* component : this->components)
		component->Start();
}

void GameObject::Update() {
	for (Component* component : this->components)
		component->Update();
}
