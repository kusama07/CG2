#pragma once

#include "DirectXCommon.h"
#include "Vector4.h"
#include "MyMath.h"
#include "Matrix4x4.h"
#include "MyEngine.h"
#include "VertexData.h"

class MyEngine;

class Triangle
{
public:
	void Initialize(DirectXCommon* dxCommon, MyEngine* engine, const Vector4&a, const Vector4& b, const Vector4& c,const Vector4& material);

	void Draw();

	void Finalize();

	void Update(const Vector4& material);

private:
	void SettingVertex();

	void Settingcolor();

	void Move();

private:

	Vector4 Material[4] = {};

	HRESULT hr_;

	MyEngine* engine_;

	WinApp winApp_;

	DirectXCommon* dxCommon_;

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

	VertexData* vertexData_;

	VertexData* vertexDataSprite_;

	//sprite用
	ID3D12Resource* vertexResourceSprite_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_;
	//TransformationMatrix用のリソース
	ID3D12Resource* transformationMatrixResourceSprite_;
	//データを書き込む
	Matrix4x4* transformationMatrixDataSprite_;
	//Transform
	Transform transformSprite_;
	//worldViewProjectionMatirx
	Matrix4x4 worldMatrixSprite_;
	Matrix4x4 viewMatrixSprite_;
	Matrix4x4 projectionMatrixSprite_;
	Matrix4x4 worldViewProjectionMatrixSprite_;

};