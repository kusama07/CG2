#include "DirectXCommon.h"

DirectXCommon::DirectXCommon() {
	hr_ = 0;
}

DirectXCommon::~DirectXCommon() {

}

void DirectXCommon::Initialize(HWND hwnd) {
	
	hwnd_ = hwnd;

	// DXGIデバイス初期化
	InitializeDXGIDevice();

	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();

	PullResourceSwapChain();

	CreateRTV();

	// フェンス生成
	CreateFence();

	//ImGuiの初期化
	ImGuiInitialize();
}

void DirectXCommon::PreDraw() {
	CommandKick();

}

void DirectXCommon::PostDraw() {
	Relese();
}

void DirectXCommon::InitializeDXGIDevice() {

	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr_));

	//使用するアダプタ用の変数
	useAdapter = nullptr;
	//順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++) {
		//アダプター情報の取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr_));

		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用アダプタの情報をログに出力
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;//ソフトウェアアダプタの場合は無視
	}
	//適切なアダプタがないため起動しない
	assert(useAdapter != nullptr);

	//機能レベルとログ出力
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelString[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか確認
	for (size_t i = 0; i < _countof(featureLevels); i++) {
		//採用したアダプターでデバイス生成
		hr_ = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルのデバイス生成に成功したか確認
		if (SUCCEEDED(hr_)) {
			//生成できたのでログ出力をしてループ抜け
			Log(std::format("FeatureLevel : {}\n", featureLevelString[i]));
			break;
		}
	}

	//デバイス生成失敗の為起動しない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!\n");//初期化完了ログ

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//ヤバイエラーで止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラーで止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		//解放
		infoQueue->Release();
	}
#endif // _DEBUG
}

void DirectXCommon::ImGuiInitialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd_);
	ImGui_ImplDX12_Init(device_,
		swapChainDesc_.BufferCount,
		rtvDesc_.Format,
		srvDescriptorHeap_,
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());
}

void DirectXCommon::InitializeCommand() {

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成失敗の為、起動しない
	assert(SUCCEEDED(hr_));

	//コマンドアロケータの生成
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr_));

	//コマンドリストを生成する
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_, nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリスト生成失敗の為、起動しない
	assert(SUCCEEDED(hr_));
}

void DirectXCommon::CreateSwapChain() {
	//スワップチェーン
	swapChainDesc_.Width = static_cast<UINT>(winApp_.GetWidth());
	swapChainDesc_.Height = static_cast<UINT>(winApp_.GetHeight());

	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc_.SampleDesc.Count = 1;
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc_.BufferCount = 2;
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成
	hr_ = dxgiFactory->CreateSwapChainForHwnd(commandQueue_, hwnd_, &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain_));
	assert(SUCCEEDED(hr_));

	rtvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	srvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

}

void DirectXCommon::PullResourceSwapChain(){

	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	//取得できなければ起動しない
	assert(SUCCEEDED(hr_));

	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(hr_));
}

ID3D12DescriptorHeap* DirectXCommon::CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr_ = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタヒープが生成失敗の為、起動しない
	assert(SUCCEEDED(hr_));

	return descriptorHeap;
}

void DirectXCommon::CreateRTV() {
	//RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2Dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	//まず一つ目を作る。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0], &rtvDesc_, rtvHandles_[0]);
	//2つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	device_->CreateRenderTargetView(swapChainResources_[1], &rtvDesc_, rtvHandles_[1]);
}

void DirectXCommon::CreateFence() {
	//初期値0でFenceを作る
	hr_ = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	//Fenceのsignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, false, false, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommon::CommandKick(){

}


void DirectXCommon::Relese() {
	CloseHandle(fenceEvent_);
	fence_->Release();

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	rtvDescriptorHeap_->Release();
	srvDescriptorHeap_->Release();
	swapChainResources_[0]->Release();
	swapChainResources_[1]->Release();
	swapChain_->Release();
	commandList_->Release();
	commandAllocator_->Release();
	commandQueue_->Release();
	device_->Release();
	useAdapter->Release();
	dxgiFactory->Release();
	CloseWindow(hwnd_);
	////リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}
WinApp DirectXCommon::winApp_;

HWND DirectXCommon::hwnd_;

//D3D12Deviceの生成
ID3D12Device* DirectXCommon::device_;

//コマンドキュー生成
ID3D12CommandQueue* DirectXCommon::commandQueue_;

//コマンドアロケータの生成
ID3D12CommandAllocator* DirectXCommon::commandAllocator_;

//コマンドリストを生成する
ID3D12GraphicsCommandList* DirectXCommon::commandList_;

//スワップチェーン
IDXGISwapChain4* DirectXCommon::swapChain_;
DXGI_SWAP_CHAIN_DESC1 DirectXCommon::swapChainDesc_{};

ID3D12DescriptorHeap* DirectXCommon::rtvDescriptorHeap_;
D3D12_RENDER_TARGET_VIEW_DESC DirectXCommon::rtvDesc_{};

ID3D12DescriptorHeap* DirectXCommon::srvDescriptorHeap_;
//RTVを２つ作るのでディスクリプタを２つ用意
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::rtvHandles_[2];
ID3D12Resource* DirectXCommon::swapChainResources_[2];

//Fence
ID3D12Fence* DirectXCommon::fence_;

HRESULT DirectXCommon::hr_;

