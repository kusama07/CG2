#include "Triangle.h"
#include <cassert>

void Triangle::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	//TriangleのvertexBufferViewを作成
	VertexBufferViewTriangle();

	//spriteのvertexBufferViewを作成
	VertexBufferViewSprite();

	//球のvertexBufferViewを作成
	VertexBufferViewSphere();

	Settingcolor();
	wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));

	descriptorSizeSRV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	for (int i = 0; i < kMaxTexture; i++)
	{
		checkTextureIndex[i] = false;
		textureResource_[i] = nullptr;
		intermediateResource_[i] = nullptr;
	}
}

void Triangle::Update(const Vector4& material) {
	Move();
	*materialData_ = material;
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

	for (int i = 0; i < kMaxTexture; i++) {

		if (checkTextureIndex[i] == true)
		{
			textureResource_[i]->Release();
			intermediateResource_[i]->Release();
		}
	}
}

void Triangle::Move() {

	transform_.rotate.y += 0.02f;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	*wvpData_ = worldMatrix_;

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

void Triangle::VertexBufferViewTriangle()
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

}

void Triangle::DrawTriangle(const Vector4& a, const Vector4& b, const Vector4& c, const ID3D12Resource& material, const Matrix4x4& viewProjectionMatrix, const int index)
{
	//transformの初期化

	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//左下
	vertexData_[0].position = { 0.0f,0.0f,0.0f,0.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };

	//上
	vertexData_[1].position = { 0.0f,0.0f,0.0f,0.0f };
	vertexData_[1].texcoord = { 0.5f,0.0f };

	//右下
	vertexData_[2].position = { 0.0f,0.0f,0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };

	vertexData_[3].position = { -0.5f,-0.8f,0.5f,1.0f };
	vertexData_[3].texcoord = { 0.0f,1.0f };

	vertexData_[4].position = { 0.0f,-0.4f,0.0f,1.0f };
	vertexData_[4].texcoord = { 0.5f,0.0f };

	vertexData_[5].position = { 0.5f,-0.8f,-0.5f,1.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };

	///三角形の描画
	//VBVを設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定。PS0にせっていしているものとはまた別。同じものを設定する
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootPrameter[2]である。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_[index]);

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);

}

void Triangle::VertexBufferViewSprite() {

	//Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSprite_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));

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

}

void Triangle::DrawSprite(const Vector4& leftTop, const Vector4& rightTop, const Vector4& leftBottom, const Vector4& rightBottom) {

	transformSprite_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//1枚目の三角形
	vertexDataSprite_[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexDataSprite_[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite_[0].normal = { 0.0f,0.0f,-1.0f };

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

	//データを書き込むためのアドレスを取得
	transformationMatrixResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite_));

	//単位行列を書き込んでおく
	*transformationMatrixDataSprite_ = MakeIdentity4x4();

	//Spriteの描画。変更が必要なものだけ変更する
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);

	//TransformationMatrixBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite_->GetGPUVirtualAddress());

	//sprite描画
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);

};

void Triangle::VertexBufferViewSphere()
{
	vertexResourceSphere_ = CreateBufferResource(dxCommon_->GetDevice() , sizeof(VertexData) * 6 * kSubdivison_ * kSubdivison_);

	transformationMatrixResourceSphere_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));

	vertexResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere_));

	vertexBufferViewSphere_.BufferLocation = vertexResourceSphere_->GetGPUVirtualAddress();

	vertexBufferViewSphere_.SizeInBytes = sizeof(VertexData) * 6 * kSubdivison_ * kSubdivison_;

	vertexBufferViewSphere_.StrideInBytes = sizeof(VertexData);

}

void Triangle::DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const int index) {

	//経度分割1つ分の角度φ
	const float kLonEvery = float(std::numbers::pi) * 2.0f / float(kSubdivison_);
	//緯度分割1つ分の角度θ
	const float kLatEvery = float(std::numbers::pi) / float(kSubdivison_);

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
			vertexDataSphere_[start].normal.x = vertexDataSphere_[start].position.x;
			vertexDataSphere_[start].normal.y = vertexDataSphere_[start].position.y;
			vertexDataSphere_[start].normal.z = vertexDataSphere_[start].position.w;

			vertexDataSphere_[start + 1].position.x = cos(lat + kLatEvery) * cos(lon) + sphere.center.x;
			vertexDataSphere_[start + 1].position.y = sin(lat + kLatEvery) + sphere.center.y;
			vertexDataSphere_[start + 1].position.w = cos(lat + kLatEvery) * sin(lon) + sphere.center.z;
			vertexDataSphere_[start + 1].position.h = 1.0f;
			vertexDataSphere_[start + 1].texcoord = { u,v };
			vertexDataSphere_[start + 1].normal.x = vertexDataSphere_[start + 1].position.x;
			vertexDataSphere_[start + 1].normal.y = vertexDataSphere_[start + 1].position.y;
			vertexDataSphere_[start + 1].normal.z = vertexDataSphere_[start + 1].position.w;

			vertexDataSphere_[start + 2].position.x = cos(lat) * cos(lon + kLonEvery) + sphere.center.x;
			vertexDataSphere_[start + 2].position.y = sin(lat) + sphere.center.y;
			vertexDataSphere_[start + 2].position.w = cos(lat) * sin(lon + kLonEvery) + sphere.center.z;
			vertexDataSphere_[start + 2].position.h = 1.0f;
			vertexDataSphere_[start + 2].texcoord = { u + uvLength, v + uvLength };
			vertexDataSphere_[start + 2].normal.x = vertexDataSphere_[start + 2].position.x;
			vertexDataSphere_[start + 2].normal.y = vertexDataSphere_[start + 2].position.y;
			vertexDataSphere_[start + 2].normal.z = vertexDataSphere_[start + 2].position.w;

			vertexDataSphere_[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery) + sphere.center.x;
			vertexDataSphere_[start + 3].position.y = sin(lat + kLatEvery) + sphere.center.y;
			vertexDataSphere_[start + 3].position.w = cos(lat + kLatEvery) * sin(lon + kLonEvery) + sphere.center.z;
			vertexDataSphere_[start + 3].position.h = 1.0f;
			vertexDataSphere_[start + 3].texcoord = { u + uvLength,v };
			vertexDataSphere_[start + 3].normal.x = vertexDataSphere_[start + 3].position.x;
			vertexDataSphere_[start + 3].normal.y = vertexDataSphere_[start + 3].position.y;
			vertexDataSphere_[start + 3].normal.z = vertexDataSphere_[start + 3].position.w;

			vertexDataSphere_[start + 4].position.x = cos(lat) * cos(lon + kLonEvery) + sphere.center.x;
			vertexDataSphere_[start + 4].position.y = sin(lat) + sphere.center.y;
			vertexDataSphere_[start + 4].position.w = cos(lat) * sin(lon + kLonEvery) + sphere.center.z;
			vertexDataSphere_[start + 4].position.h = 1.0f;
			vertexDataSphere_[start + 4].texcoord = { u + uvLength, v + uvLength };
			vertexDataSphere_[start + 4].normal.x = vertexDataSphere_[start + 4].position.x;
			vertexDataSphere_[start + 4].normal.y = vertexDataSphere_[start + 4].position.y;
			vertexDataSphere_[start + 4].normal.z = vertexDataSphere_[start + 4].position.w;


			vertexDataSphere_[start + 5].position.x = cos(lat + kLatEvery) * cos(lon) + sphere.center.x;
			vertexDataSphere_[start + 5].position.y = sin(lat + kLatEvery) + sphere.center.y;
			vertexDataSphere_[start + 5].position.w = cos(lat + kLatEvery) * sin(lon) + sphere.center.z;
			vertexDataSphere_[start + 5].position.h = 1.0f;
			vertexDataSphere_[start + 5].texcoord = { u,v };
			vertexDataSphere_[start + 5].normal.x = vertexDataSphere_[start + 5].position.x;
			vertexDataSphere_[start + 5].normal.y = vertexDataSphere_[start + 5].position.y;
			vertexDataSphere_[start + 5].normal.z = vertexDataSphere_[start + 5].position.w;

		}	

		//データを書き込むためのアドレスを取得
		transformationMatrixResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSphere_));

		worldMatrixSphere_ = MakeAffineMatrix(transformSphere_.scale, transformSphere_.rotate, transformSphere_.translate);

		//単位行列を書き込んでおく
		*transformationMatrixDataSphere_ = Multiply(worldMatrixSphere_,viewProjectionMatrix);

		//球の描画
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

		//球の描画。変更が必要なものだけ変更する
		dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSphere_);

		//TransformationMatrixBufferの場所を設定
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSphere_->GetGPUVirtualAddress());

		//SRVのDescriptorTableの先頭を設定。2はrootPrameter[2]である。
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_[index]);

		//sphere描画
		dxCommon_->GetCommandList()->DrawInstanced(kSubdivison_ * kSubdivison_ * 6, 1, 0, 0);
	}
}

DirectX::ScratchImage Triangle::createmap(const std::string& filePath)
{
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの生成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}

ID3D12Resource* Triangle::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	//metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

ID3D12Resource* Triangle::UploadTexturData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	std::vector<D3D12_SUBRESOURCE_DATA>subresources;
	DirectX::PrepareUpload(dxCommon_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(dxCommon_->GetDevice(), intermediateSize);
	UpdateSubresources(dxCommon_->GetCommandList(), texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	//Textureへの転送後は利用できるように、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

int Triangle::LoadTexture(const std::string& filePath)
{
	int spriteIndex = 0;

	for (int i = 0; i < kMaxTexture; ++i) {
		if (checkTextureIndex[i] == false) {
			spriteIndex = i;
			checkTextureIndex[i] = true;

			break;
		}
	}

	if (spriteIndex < 0) {
		assert(false);
	}

	if (kMaxTexture < spriteIndex) {
		assert(false);
	}

	DirectX::ScratchImage mipImages = createmap(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textureResource_[spriteIndex] = CreateTextureResource(dxCommon_->GetDevice(), metadata);
	intermediateResource_[spriteIndex] = UploadTexturData(textureResource_[spriteIndex], mipImages);

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU_[spriteIndex] = GetCPUDescriptorHandle(dxCommon_->GetsrvDescriptor(), descriptorSizeSRV, spriteIndex);
	textureSrvHandleGPU_[spriteIndex] = GetGPUDescriptorHandle(dxCommon_->GetsrvDescriptor(), descriptorSizeSRV, spriteIndex);

	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU_[spriteIndex].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_[spriteIndex].ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//SRVの作成
	dxCommon_->GetDevice()->CreateShaderResourceView(textureResource_[spriteIndex], &srvDesc, textureSrvHandleCPU_[spriteIndex]);

	return spriteIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE Triangle::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{	
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE Triangle::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}
