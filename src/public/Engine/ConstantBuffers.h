#pragma once
#include "DirectXIncludes.h"

/*!
	WVP:
		- World
		- View
		- Projection
*/
struct WVP {
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
};