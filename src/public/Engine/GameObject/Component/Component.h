#pragma once
#include <iostream>

class Component {
public:
	explicit Component() = default;
	virtual void Start();
	virtual void Update();
};