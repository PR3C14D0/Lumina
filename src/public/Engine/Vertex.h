#pragma once

typedef float RG[2];
typedef float RGB[3];
typedef float RGBA[4];

struct Vertex {
	RGB position;
	RGB normal;
	RG uv;
};

struct ScreenQuadVertex {
	RGB position;
};