#include "Triangle.h"
#include <cassert>

void Triangle::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	//TriangleのvertexBufferViewを作成
	VertexBufferViewTriangle();

	//spriteのvertexBufferViewを作成
	VertexBufferViewSprite();
	IndexBufferViewSprite();

	//球のvertexBufferViewを作成
	VertexBufferViewSphere();

	Settingcolor();
	wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));


	//materialResourceObj_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	//transformationMatrixResourceObj_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));

	// directionalLightにBufferResourceを作る
	directionalLightResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionlLight));
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// デフォルト値はとりあえず以下のようにしておく
	directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_->intensity = 1.0f;

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

void Triangle::Update(const Vector4& material,const Vector3& direction) {
	Move();
	*materialData_ = material;
	// DirectionalLight
	directionalLightData_->direction = direction;

}

void Triangle::Finalize()
{
	materialResourceTriangle_->Release();
	vertexResource_->Release();

	vertexResourceSprite_->Release();
	transformationMatrixResourceSprite_->Release();
	wvpResource_->Release();

	vertexResourceSphere_->Release();
	transformationMatrixResourceSphere_->Release();

	materialResourceSprite_->Release();
	materialResourceSphere_->Release();

	directionalLightResource_->Release();

	indexResourceSprite_->Release();

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
	materialResourceTriangle_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	//書き込むためのアドレスを取得	
	materialResourceTriangle_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
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

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceTriangle_->GetGPUVirtualAddress());

	// マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootPrameter[2]である。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_[index]);

	// Lightingの場所設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);

}

void Triangle::VertexBufferViewSprite() {


	//Sprite用の頂点リソースを作る
	vertexResourceSprite_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 4);

	// materialResource作成
	materialResourceSprite_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material) * kMaxSprite_);

	//Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResourceSprite_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));
	
	//index用のResourcesを作成
	indexResourceSprite_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * kMaxSpriteVertex_);

	//Sprite
	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));

	//リソースの先頭のアドレスから使う
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();

	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;

	//1頂点当たりのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);


}

void Triangle::IndexBufferViewSprite() {
	//リソースの先頭のアドレスから使う
	indexBufferViewSprite_.BufferLocation = indexResourceSprite_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite_.SizeInBytes = sizeof(uint32_t) * kMaxSpriteVertex_;
	//インデックスはuint32_tとする
	indexBufferViewSprite_.Format = DXGI_FORMAT_R32_UINT;

}

void Triangle::DrawSprite(const Vector4& leftTop, const Vector4& leftBottom, const Vector4& rightTop, const Vector4& rightBottom, int index, const Vector4& material) {
	int spriteIndex = kMaxSpriteVertex_ + 1;

	for (int i = 0; i < kMaxSprite_; ++i)
	{
		if (CheckSpriteIndex_[i] == false)
		{
			spriteIndex = (i * 6);
			CheckSpriteIndex_[i] = true;
			break;
		}
	}

	if (spriteIndex < 0)
	{
		assert(false);
	}

	if (kMaxSpriteVertex_ < spriteIndex)
	{
		assert(false);
	}

	//1枚目の三角形
	vertexDataSprite_[0].position = leftBottom;//左下
	vertexDataSprite_[0].texcoord = { 0.0f,1.0f };

	vertexDataSprite_[1].position = leftTop;//左上
	vertexDataSprite_[1].texcoord = { 0.0f,0.0f };

	vertexDataSprite_[2].position = rightBottom;//右下
	vertexDataSprite_[2].texcoord = { 1.0f,1.0f };

	vertexDataSprite_[3].position = rightTop;//右上
	vertexDataSprite_[3].texcoord = { 1.0f,0.0f };

	//インデックスリソースにデータを書き込む
	indexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite_));

	indexDataSprite_[spriteIndex] = 0;
	indexDataSprite_[spriteIndex + 1] = 1;
	indexDataSprite_[spriteIndex + 2] = 2;
	indexDataSprite_[spriteIndex + 3] = 1;
	indexDataSprite_[spriteIndex + 4] = 3;
	indexDataSprite_[spriteIndex + 5] = 2;

	// materialResourceをMapしてデータを書き込む。
	materialResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite_));

	//色指定
	materialDataSprite_->color = material;

	// materialにLightingを有効にするかどうか
	materialDataSprite_->enableLighting = false;

	//データを書き込むためのアドレスを取得
	transformationMatrixResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite_));
	//sprite用のwroldviewProjectionMatirxを作る
	worldMatrixSprite_ = MakeAffineMatrix(transformSprite_.scale, transformSprite_.rotate, transformSprite_.translate);
	viewMatrixSprite_ = MakeIdentity4x4();
	projectionMatrixSprite_ = MakeOrthoGraphicMatrix(0.0f, 0.0f, float(winApp_.GetWidth()), float(winApp_.GetHeight()), 0.0f, 100.0f);
	worldViewProjectionMatrixSprite_ = Multiply(worldMatrixSprite_, Multiply(viewMatrixSprite_, projectionMatrixSprite_));
	//単位行列を書き込んでおく
	transformationMatrixDataSprite_->WVP = worldViewProjectionMatrixSprite_;						 
	transformationMatrixDataSprite_->World = MakeIdentity4x4();

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Spriteの描画。変更が必要なものだけ変更する
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);
	//indexのIBVを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite_);


	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite_->GetGPUVirtualAddress());

	//TransformationMatrixBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootPrameter[2]である。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_[index]);

	// Lightingの場所設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//描画(DrawCall)6個のインデックスを利用し1つのインスタンスを描画。その他は当面Θで良い
	dxCommon_->GetCommandList()->DrawIndexedInstanced(spriteIndex + 6, 1, 0, 0, 0);

};

void Triangle::VertexBufferViewSphere()
{
	//vertexResourceSphere_ = CreateBufferResource(dxCommon_->GetDevice() , sizeof(VertexData) * 6 * kSubdivison_ * kSubdivison_);

	transformationMatrixResourceObj_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));

	// materialResource作成
	materialResourceObj_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	/*vertexResourceSphere_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere_));

	vertexBufferViewSphere_.BufferLocation = vertexResourceSphere_->GetGPUVirtualAddress();

	vertexBufferViewSphere_.SizeInBytes = sizeof(VertexData) * 6 * kSubdivison_ * kSubdivison_;

	vertexBufferViewSphere_.StrideInBytes = sizeof(VertexData);*/

}

void Triangle::DrawModel(const Matrix4x4& viewProjectionMatrix, const int index, const Vector4& material) {

	//************OBJ
	ModelData modelData = LoadObjFile("resource", "plane.obj");

	vertexResourceObj_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData.vertices.size());

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewObj{};
	vertexBufferViewObj.BufferLocation = vertexResourceObj_->GetGPUVirtualAddress();
	vertexBufferViewObj.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferViewObj.StrideInBytes = sizeof(VertexData);

	vertexDataObj_ = nullptr;
	vertexResourceObj_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataObj_));
	std::memcpy(vertexDataObj_, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	// MaterialResourceをMapしてデータを書き込む。
	materialResourceObj_->Map(0, nullptr, reinterpret_cast<void**>(&materialDataObj_));

	// materialにlightingを有効にするかどうか
	materialDataObj_->enableLighting = false;
	//色指定
	materialDataObj_->color = material;
	materialDataObj_->uvTransform = MakeIdentity4x4();

	//データを書き込むためのアドレスを取得
	transformationMatrixResourceObj_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataObj_));

	worldMatrixObj_ = MakeAffineMatrix(transformObj_.scale, transformObj_.rotate, transformObj_.translate);

	//単位行列を書き込んでおく
	transformationMatrixDataObj_->WVP = Multiply(worldMatrixObj_, viewProjectionMatrix);

	transformationMatrixDataObj_->World = MakeIdentity4x4();


	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//変更が必要なものだけ変更する
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewObj);

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceObj_->GetGPUVirtualAddress());

	//TransformationMatrixBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceObj_->GetGPUVirtualAddress());

	//SRVのDescriptorTableの先頭を設定。2はrootPrameter[2]である。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_[index]);

	// Lightingの場所設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//描画
	dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

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

void Triangle::ResetVertex() {

	for (int i = 0; i < 3; ++i)
	{

		if (CheckSpriteIndex_[i] == true)
		{
			CheckSpriteIndex_[i] = false;
		}
	}
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

ModelData Triangle::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.w;
			position.x *= -1.0f;
			position.w *= 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangleData[3];

			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}

				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position,texcoord,normal };
				modelData.vertices.push_back(vertex);

				triangleData[faceVertex] = { position,texcoord,normal };
			}
			modelData.vertices.push_back(triangleData[2]);
			modelData.vertices.push_back(triangleData[1]);
			modelData.vertices.push_back(triangleData[0]);
		}
	}

	return modelData;
}
