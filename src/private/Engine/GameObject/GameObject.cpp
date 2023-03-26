#include "Engine/GameObject/GameObject.h"

GameObject::GameObject(std::string name) {
	this->name = name;
	this->transform = Transform{};
}

void GameObject::Start() {
	for (Component* component : this->components)
		component->Start();
}

void GameObject::Update() {
	for (Component* component : this->components)
		component->Update();
}

/*!
	This method will get you a component that is added to the GameObject.
		Usage: You need to simply pass the type of the component.
		Example: GameObject::GetComponent<Mesh>(); This call will return the mesh component.
*/
template<typename T>
T* GameObject::GetComponent() {
	T* retComp = nullptr;
	for (Component* component : this->components) {
		T* actualComp = dynamic_cast<T*>(component);
		if (actualComp) {
			retComp = actualComp;
			break;
		}
	}

	if (!retComp)
		MessageBox(NULL, "No component with that type found on this GameObjec", "Error", MB_OK);

	return retComp;
}

/*!
	This method will add a component to our GameObject.
		Usage: You need to simply pass the type of the component and a pointer to the component.
		Example: GameObject::AddComponent<Mesh>(myMeshComponent); This call will add a mesh component.
*/
template<typename T>
void GameObject::AddComponent(Component* component) {
	T* workingComp = dynamic_cast<T*>(component);
	
	/* Verify if the component is repeated. */
	bool bRepeatedComp = false;
	for (Component* component : this->components) {
		T* actualComp = dynamic_cast<T*>(component);
		if (actualComp) {
			bRepeatedComp = true;
			break;
		}
	}

	if (bRepeatedComp) {
		MessageBox(NULL, "A component of that type already exists", "Error", MB_OK);
		return;
	}

	this->components.push_back(component);
	return;
}