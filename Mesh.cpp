#include "Mesh.h"

void Mesh::Initialize() {
	
}

void Mesh::Update() {

}

void Mesh::Draw(const Vector4& a, const Vector4& b, const Vector4& c) {
	Sprite_->DrawTriangle(a, b, c);
}