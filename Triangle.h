#pragma once

#include "DirectXCommon.h"
#include "Vector4.h"

class Sprite;

class Triangle
{
public:
	void Initialize(DirectXCommon* dxCommon, const Vector4&a, const Vector4& b, const Vector4& c,const Vector4& material);

	void Draw();

	void Finalize();

private:
	void SettingVertex();

	void Settingcolor();

private:

	HRESULT hr_;

	Sprite* Sprite_;

	DirectXCommon* dxCommon_;

	Vector4* vertexData_;

	Vector4* materialData_;

	ID3D12Resource* vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	ID3D12Resource* materialResource_;

	ID3D12Resource* Resource_;

	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};

	D3D12_RESOURCE_DESC vertexResourceDesc{};
};