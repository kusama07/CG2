#pragma once
#include "Vector4.h"
#include "Triangle.h"
#include "Sprite.h"

class Mesh
{
public:

	void Initialize();
	void Update();
	void Draw(const Vector4& a, const Vector4& b, const Vector4& c);

private:
	static	DirectXCommon* dxClass_;
	int triangleCount_;
	Triangle* triangle_[10];

	Sprite* Sprite_;

};

