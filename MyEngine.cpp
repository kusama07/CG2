#include "MyEngine.h"


MyEngine::MyEngine()
{
	fenceValue_ = dxCommon_->GetFenceValue();
}

MyEngine::~MyEngine() {}

void MyEngine::Initialize()
{
	DXCInitialize();
	CreateRootSignature();
	SettingInputLayout();
	SettingBlendState();
	SettingRasterizerState();
	ShaderCompile();
	CreatePSO();
	VertexResource();

}

void MyEngine::Update()
{
	dxCommon_->PreDraw();
	Render();
	ImGui::ShowDemoWindow();
}

void MyEngine::UpdateEnd()
{
	ImGui::Render();
	StateChange();
}

void MyEngine::Finalize()
{
	Relese();
}


IDxcBlob* MyEngine::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
	LPCWSTR arguments[] =
	{
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProflieの設定
		L"-Od", //最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};

	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile
	(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler, // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//警告・エラーが出たらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	if (shaderResult != 0)
	{
		shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
		if (shaderError != nullptr && shaderError->GetStringLength() != 0)
		{
			Log(shaderError->GetStringPointer());
			//警告・エラーダメゼッタイ
			assert(false);
		}
	}
	
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

void MyEngine::DXCInitialize()
{
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr_));
	//現時点でincludeはしないが、includeに対応するための設定を行っていく
	hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr_));
}

void MyEngine::CreateRootSignature()
{
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成、複数設定できるので配列、今回は結果は１つだけなので長さ１の配列
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	descriptionRootSignature_.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature_.NumParameters = _countof(rootParameters);//配列の長さ

	//シリアライズしてバイナリにする
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_))
	{
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr_ = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr_));
}

void MyEngine::SettingInputLayout()
{
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void MyEngine::SettingBlendState()
{
	//すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
}

void MyEngine::SettingRasterizerState()
{
	//裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

}

void MyEngine::ShaderCompile() {
	//Shaderをコンパイルする
	vertexShaderBlob_ = CompileShader(L"Object3d.VS.hlsl",
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob_ != nullptr);


	pixelShaderBlob_ = CompileShader(L"Object3d.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
}

void MyEngine::CreatePSO()
{
	graphicsPipelineStateDesc_.pRootSignature = rootSignature_;//RootSignature
	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc_;//Inputlayout
	graphicsPipelineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize() };//pixcelShader
	graphicsPipelineStateDesc_.BlendState = blendDesc_;//BlendState
	graphicsPipelineStateDesc_.RasterizerState = rasterizerDesc_;//rasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc_.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定（気にしなく良い）
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//実際に生成

	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr_));
}

void MyEngine::VertexResource()
{
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc_.Width = sizeof(Vector4) * 3;//リソースサイズ　今回はvector4を四分割
	//バッファの場合はこれらは１にする決まり
	vertexResourceDesc_.Height = 1;
	vertexResourceDesc_.DepthOrArraySize = 1;
	vertexResourceDesc_.MipLevels = 1;
	vertexResourceDesc_.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	vertexResourceDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//実際に頂点リソースを作る
	hr_ = dxCommon_->GetDevice()->CreateCommittedResource(&uplodeHeapProperties,
		D3D12_HEAP_FLAG_NONE, &vertexResourceDesc_, D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, IID_PPV_ARGS(&vertexResource_));

	assert(SUCCEEDED(hr_));
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	//1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

}

void MyEngine::Render()
{

	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>(winApp_.GetWidth());
	viewport_.Height = static_cast<float>(winApp_.GetHeight());

	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = winApp_.GetWidth();
	scissorRect_.top = 0;
	scissorRect_.bottom = winApp_.GetHeight();

	//これから書き込むバッグバッファのインデックスを取得
	backBufferIndex = dxCommon_->GetSwapChain()->GetCurrentBackBufferIndex();

	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象リソース。現在のバッグバッファに対して行う
	barrier.Transition.pResource = dxCommon_->GetSwapChainResource(backBufferIndex);
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	//描画先のRTVを設定する
	dxCommon_->GetCommandList()->OMSetRenderTargets(1, &dxCommon_->GetRTVHandle(backBufferIndex), false, nullptr);
	//指定した色で画面全体をクリアする
	float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
	dxCommon_->GetCommandList()->ClearRenderTargetView(dxCommon_->GetRTVHandle(backBufferIndex), clearcolor, 0, nullptr);

	ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon_->GetsrvDescriptor()};
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	//コマンドを積む
	dxCommon_->GetCommandList()->RSSetViewports(1, &viewport_);	//Viewportを設定
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);	//Scirssorを設定
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_);
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_);	//PSOを設定

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	//VBVを設定

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void MyEngine::StateChange()
{
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());


	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	dxCommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	hr_ = dxCommon_->GetCommandList()->Close();

	assert(SUCCEEDED(hr_));

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { dxCommon_->GetCommandList() };
	dxCommon_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	dxCommon_->GetSwapChain()->Present(1, 0);

	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	dxCommon_->GetCommandQueue()->Signal(dxCommon_->GetFence(), fenceValue_);

	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (dxCommon_->GetFence()->GetCompletedValue() < fenceValue_)
	{
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		dxCommon_->GetFence()->SetEventOnCompletion(fenceValue_, dxCommon_->GetFenceEvent());
		//イベント待つ
		WaitForSingleObject(dxCommon_->GetFenceEvent(), INFINITE);
	}

	//次のフレーム用のコマンドリストを準備
	hr_ = dxCommon_->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = dxCommon_->GetCommandList()->Reset(dxCommon_->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(hr_));

}

void MyEngine::Relese()
{

	vertexResource_->Release();
	graphicsPipelineState_->Release();
	signatureBlob_->Release();
	if (errorBlob_)
	{
		errorBlob_->Release();
	}

	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();
}

DirectX::ScratchImage MyEngine::LoadTexture(const std::string& filePath)
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