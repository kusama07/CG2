#pragma once

#include "DirectXCommon.h"
#include "Vector4.h"
#include "MyMath.h"
#include "Matrix4x4.h"
#include "MyEngine.h"

class MyEngine;

class Triangle
{
public:
	void Initialize(DirectXCommon* dxCommon, const Vector4&a, const Vector4& b, const Vector4& c,const Vector4& material);

	void Draw();

	void Finalize();

	void Update();

private:
	void SettingVertex();

	void Settingcolor();

	void Move();

private:

	HRESULT hr_;

	MyEngine* engine_;

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

	ID3D12Resource* wvpResource_;

	Matrix4x4* wvpData_;

	Matrix4x4 worldMatrix_;

	Transform transform_;

};