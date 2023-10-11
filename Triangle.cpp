#include "Triangle.h"
#include <cassert>

void Triangle::Initialize(DirectXCommon* dxCommon, MyEngine* engine, const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& material)
{
	dxCommon_ = dxCommon;
	engine_ = engine;
	SettingVertex();
	Settingcolor();

	//transformの初期化

	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	transformSprite_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Sphere sphere = { 0.0f, 0.0f, 0.0f, 1.0f };

	//左下
	vertexData_[0].position = a;	
	vertexData_[0].texcoord = { 0.0f,1.0f };

	//上
	vertexData_[1].position = b;
	vertexData_[1].texcoord = { 0.5f,0.0f };

	//右下
	vertexData_[2].position = c;
	vertexData_[2].texcoord = { 1.0f,1.0f };

	vertexData_[3].position = { -0.5f,-0.8f,0.5f,1.0f };
	vertexData_[3].texcoord = { 0.0f,1.0f };

	vertexData_[4].position = { 0.0f,-0.4f,0.0f,1.0f };
	vertexData_[4].texcoord = { 0.5f,0.0f };

	vertexData_[5].position = { 0.5f,-0.8f,-0.5f,1.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };

	//******************sprite用****************//
	
	//1枚目の三角形
	vertexDataSprite_[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexDataSprite_[0].texcoord = { 0.0f,1.0f };
	
	vertexDataSprite_[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexDataSprite_[1].texcoord = { 0.0f,0.0f };

	vertexDataSprite_[2].position = { 640.0f,360.0f,0.0f,1.0f };//右上
	vertexDataSprite_[2].texcoord = { 1.0f,1.0f };

	//2枚目の三角形
	vertexDataSprite_[3].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexDataSprite_[3].texcoord = { 0.0f,0.0f };

	vertexDataSprite_[4].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexDataSprite_[4].texcoord = { 1.0f,0.0f };

	vertexDataSprite_[5].position = { 640.f,360.0f,0.0f,1.0f };//右上
	vertexDataSprite_[5].texcoord = { 1.0f,1.0f };

	//************************************//

	*materialData_ = material;

	wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));

	//**************************sprite******************//
	//Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSprite_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));

	//データを書き込むためのアドレスを取得
	transformationMatrixResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite_));
	
	//谷行列を書き込んでおく
	*transformationMatrixDataSprite_ = MakeIdentity4x4();

	//Sprite用のworldViewProjectionMatrixを作る
	worldMatrixSprite_ = MakeAffineMatrix(transformSprite_.scale, transformSprite_.rotate, transformSprite_.translate);
	viewMatrixSprite_ = MakeIdentity4x4();
	projectionMatrixSprite_ = MakeOrthoGraphicMatrix(0.0f, 0.0f, float(winApp_.GetWidth()), float(winApp_.GetHeight()), 0.0f, 100.0f);
	worldViewProjectionMatrixSprite_ = Multiply(worldMatrixSprite_, Multiply(viewMatrixSprite_, projectionMatrixSprite_));
	*transformationMatrixDataSprite_ = worldViewProjectionMatrixSprite_;

	//**************************************************//

	//************球***********//
	SettingSphereVertex(sphere, projectionMatrixSphere_);

	//Sphere用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSphere_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));

	//データを書き込むためのアドレスを取得
	transformationMatrixResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSphere_));

	//谷行列を書き込んでおく
	*transformationMatrixDataSphere_ = MakeIdentity4x4();

	//Sprite用のworldViewProjectionMatrixを作る
	worldMatrixSprite_ = MakeAffineMatrix(transformSprite_.scale, transformSprite_.rotate, transformSprite_.translate);
	viewMatrixSprite_ = MakeIdentity4x4();
	projectionMatrixSphere_ = MakeOrthoGraphicMatrix(0.0f, 0.0f, float(winApp_.GetWidth()), float(winApp_.GetHeight()), 0.0f, 100.0f);
	worldViewProjectionMatrixSphere_ = Multiply(worldMatrixSphere_, Multiply(viewMatrixSphere_, projectionMatrixSphere_));
	*transformationMatrixDataSphere_ = worldViewProjectionMatrixSphere_;

	//*************************//

	*wvpData_ = MakeIdentity4x4();

	worldMatrix_ = MakeIdentity4x4();


}

void Triangle::Update(const Vector4& material) {
	Move();
	*materialData_ = material;

}

void Triangle::Draw()
{
	///三角形の描画
	//VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定。PS0にせっていしているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootPrameter[2]である。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, engine_->GetTextureHandleGPU());

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);

	/// *********Spriteの描画
	//Spriteの描画。変更が必要なものだけ変更する
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);

	//TransformationMatrixBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite_->GetGPUVirtualAddress());

	//sprite描画
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);

	///**********球の描画
	//球の描画。変更が必要なものだけ変更する
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSphere_);

	//TransformationMatrixBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSphere_->GetGPUVirtualAddress());

	//sphere描画
	dxCommon_->GetCommandList()->DrawInstanced(kSubdivison_ * kSubdivison_ * 6, 1, 0, 0);

}

void Triangle::Finalize()
{
	materialResource_->Release();
	vertexResource_->Release();

	vertexResourceSprite_->Release();
	transformationMatrixResourceSprite_->Release();
	wvpResource_->Release();

	vertexResourceSphere_->Release();
	transformationMatrixResourceSphere_->Release();
}

void Triangle::SettingVertex()
{

	//三角形用の頂点リソースを作る
	vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;

	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//******************Sprite用**************//

	//Sprite用の頂点リソースを作る
	vertexResourceSprite_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();

	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 6;

	//1頂点当たりのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);

	//Sprite
	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));


	//***************************************//

	//*********************球**************************//

	//sphere用の頂点リソースを作る
	vertexResourceSphere_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 6);

	//リソースの先頭のアドレスから使う
	vertexBufferViewSphere_.BufferLocation = vertexResourceSphere_->GetGPUVirtualAddress();

	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSphere_.SizeInBytes = sizeof(VertexData) * 6;

	//1頂点当たりのサイズ
	vertexBufferViewSphere_.StrideInBytes = sizeof(VertexData);

	//sphere
	vertexResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere_));

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

	transform_.rotate.y += 0.02f;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	*wvpData_ = worldMatrix_;

}

void Triangle::SettingSphereVertex(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix) {
	//経度分割1つ分の角度φ
	const float kLonEvery = float(std::numbers::pi) * 2.0f / float(kSubdivison_);
	//緯度分割1つ分の角度θ
	const float kLatEvery = float(std::numbers::pi) / float(kSubdivison_);

	kSubdivison_ = 30;

	//緯度の方向に分割
	for (int latIndex = 0; latIndex < kSubdivison_; ++latIndex)
	{
		float lat = -float(std::numbers::pi) / 2.0f + kLatEvery * latIndex;
		float uvLength = 1.0f / kSubdivison_;

		//経度の方向に分割しながら線を描く
		for (int lonIndex = 0; lonIndex < kSubdivison_; ++lonIndex)
		{
			uint32_t start = (latIndex * kSubdivison_ + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;
			float u = float(lonIndex) / float(kSubdivison_);
			float v = 1.0f - float(latIndex) / float(kSubdivison_);

			vertexDataSphere_[start].position.x = cos(lat) * cos(lon) + sphere.center.x;
			vertexDataSphere_[start].position.y = sin(lat) + sphere.center.y;
			vertexDataSphere_[start].position.w = cos(lat) * sin(lon) + sphere.center.z;
			vertexDataSphere_[start].position.h = 1.0f;
			vertexDataSphere_[start].texcoord = { u,v + uvLength };

			vertexDataSphere_[start + 1].position.x = cos(lat + kLatEvery) * cos(lon) + sphere.center.x;
			vertexDataSphere_[start + 1].position.y = sin(lat + kLatEvery) + sphere.center.y;
			vertexDataSphere_[start + 1].position.w = cos(lat + kLatEvery) * sin(lon) + sphere.center.z;
			vertexDataSphere_[start + 1].position.h = 1.0f;
			vertexDataSphere_[start + 1].texcoord = { u,v };

			vertexDataSphere_[start + 2].position.x = cos(lat) * cos(lon + kLonEvery) + sphere.center.x;
			vertexDataSphere_[start + 2].position.y = sin(lat) + sphere.center.y;
			vertexDataSphere_[start + 2].position.w = cos(lat) * sin(lon + kLonEvery) + sphere.center.z;
			vertexDataSphere_[start + 2].position.h = 1.0f;
			vertexDataSphere_[start + 2].texcoord = { u + uvLength, v + uvLength };

			vertexDataSphere_[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery) + sphere.center.x;
			vertexDataSphere_[start + 3].position.y = sin(lat + kLatEvery) + sphere.center.y;
			vertexDataSphere_[start + 3].position.w = cos(lat + kLatEvery) * sin(lon + kLonEvery) + sphere.center.z;
			vertexDataSphere_[start + 3].position.h = 1.0f;
			vertexDataSphere_[start + 3].texcoord = { u + uvLength,v };

			vertexDataSphere_[start + 4].position.x = cos(lat) * cos(lon + kLonEvery) + sphere.center.x;
			vertexDataSphere_[start + 4].position.y = sin(lat) + sphere.center.y;
			vertexDataSphere_[start + 4].position.w = cos(lat) * sin(lon + kLonEvery) + sphere.center.z;
			vertexDataSphere_[start + 4].position.h = 1.0f;
			vertexDataSphere_[start + 4].texcoord = { u + uvLength, v + uvLength };

			vertexDataSphere_[start + 5].position.x = cos(lat + kLatEvery) * cos(lon) + sphere.center.x;
			vertexDataSphere_[start + 5].position.y = sin(lat + kLatEvery) + sphere.center.y;
			vertexDataSphere_[start + 5].position.w = cos(lat + kLatEvery) * sin(lon) + sphere.center.z;
			vertexDataSphere_[start + 5].position.h = 1.0f;
			vertexDataSphere_[start + 5].texcoord = { u,v };
		}
	}
}