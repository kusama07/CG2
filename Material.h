#pragma once

struct Material {
	Vector4 color;
	int32_t enableLighting;
	Matrix4x4 uvTransform;
};