#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <format>
#include <dxgidebug.h>
#include "WindowsClass.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

class DirectXClass {
public:

	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	static void Log(const std::string& message);

	static void DirectXInitialization();

	static void InitializeDXGIDevice();

	static void InitializeCommand();

	static void CreateSwapChain();

	static void CreateFinalRenderTargets();

	static void CreateFence();

	static void Release();

	static void ResourceCheck();

private:

	static WindowsClass* win_;

	//DXGIファクトリーの生成
	static IDXGIFactory7* dxgiFactory;
	static HRESULT hr;

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	static IDXGIAdapter4* useAdapter;

	static ID3D12Device* device;

	// コマンドキューを生成する
	static ID3D12CommandQueue* commandQueue;
	static inline D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	// コマンドアプリケータを生成する
	static ID3D12CommandAllocator* commandAllocator;

	// コマンドリストの生成
	static ID3D12GraphicsCommandList* commandList;

	//　スワップチェーンを生成
	static IDXGISwapChain4* swapChain;
	static inline DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	//デスクリプタヒープの生成
	static ID3D12DescriptorHeap* rtvDescriptorHeap;
	static inline D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};

	//スワップチェーンからリソースを引っ張ってくる
	static ID3D12Resource* swapChainResources[2];

	static ID3D12Fence* fence;
	static uint64_t fenceValue;

	static HANDLE fenceEvent;

	static ID3D12Debug1* debugController;
	static IDXGIDebug1* debug;

};
