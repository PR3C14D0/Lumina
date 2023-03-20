#include "Engine/GameObject/GameObject.h"

GameObject::GameObject(std::string name) {
	this->name = name;
	this->transform = Transform{};
}