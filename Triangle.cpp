#include "Triangle.h"
#include <cassert>

void Triangle::Initialize(DirectXCommon* dxCommon, const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material)
{
	dxCommon_ = dxCommon;
	SettingVertex();
	Settingcolor();

	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//左下
	vertexData_[0] = a;
	//上
	vertexData_[1] = b;
	//右下
	vertexData_[2] = c;

	*materialData_ = material;

	wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));

	*wvpData_ = MakeIdentity4x4();

	worldMatrix_ = MakeIdentity4x4();

}

void Triangle::Update(const Vector4& material) {
	Move();
	for (int i = 0; i < 4; i++) {
		*materialData_ = material;
	}
}

void Triangle::Draw()
{

	//VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定。PS0にせっていしているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//描画
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);

}

void Triangle::Finalize()
{
	materialResource_->Release();
	vertexResource_->Release();
	wvpResource_->Release();
}

void Triangle::SettingVertex()
{
	vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4) * 3);

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;

	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Vector4);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

}

void Triangle::Settingcolor() {
	//マテリアル用のリソースを作る　今回はcolor1つ分	
	materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	//書き込むためのアドレスを取得	
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}


ID3D12Resource* Triangle::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	//頂点リソース用のヒープの設定
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う

	//頂点リソースの設定

	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;//リソースサイズ　

	//バッファの場合はこれらは１にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	//バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//実際に頂点リソースを作る
	hr_ = dxCommon_->GetDevice()->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&Resource_));

	assert(SUCCEEDED(hr_));

	return Resource_;
}

void Triangle::Move() {

	transform_.rotate.y += 0.03f;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	*wvpData_ = worldMatrix_;

}