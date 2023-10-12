#pragma once
#include <cmath>
#include <numbers>

#include "DirectXCommon.h"
#include "Vector4.h"
#include "MyMath.h"
#include "Matrix4x4.h"
#include "MyEngine.h"
#include "VertexData.h"
#include "Sphere.h"
#include "Vector2.h"

class MyEngine;

class Triangle
{
public:
	void Initialize();
	
	void Update(const Vector4& material);

	void DrawTriangle(const Vector4& a, const Vector4& b, const Vector4& c, const ID3D12Resource& material, const Matrix4x4& viewProjectionMatrix);
	
	void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix);

	void DrawSprite(const Vector4& leftTop, const Vector4& rightTop, const Vector4& leftBottom, const Vector4& rightBottom);

	void Finalize();

private:
	void Settingcolor();

	void Move();

	void VertexBufferViewTriangle();

	void VertexBufferViewSphere();

	void VertexBufferViewSprite();

public:
	//データを書き込む
	Matrix4x4* transformationMatrixDataSphere_;

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

	///**********Sphere
	//球の頂点データを書き込む最初の場所

	//分割数
	const int kSubdivison_ = 16;

	//vertexResources
	ID3D12Resource* vertexResourceSphere_;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere_;
	//TransformationMatrix用のリソース
	ID3D12Resource* transformationMatrixResourceSphere_;
	//Transform
	Transform transformSphere_;
	//vertexData
	VertexData* vertexDataSphere_;

	ID3D12Resource* wvpResourceSphere_;
	Matrix4x4* wvpDataSphere_;

	Matrix4x4 worldMatrixSphere_;
	Matrix4x4 viewMatrixSphere_;
	Matrix4x4 projectionMatrixSphere_;
	Matrix4x4 worldViewProjectionMatrixSphere_;

};