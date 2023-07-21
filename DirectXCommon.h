#pragma once
#include <chrono>
#include <cstdlib>
#include <dxgi1_6.h>
#include "WinApp.h"
#include "ConvertString.h"

class DirectXCommon
{
public:
	void Initialization(WinApp* win, const wchar_t* title, int32_t backBufferWidth = WinApp::kClientWidth, int32_t backBufferHeight = WinApp::kClientHeight);

	void PreDraw();

	void PostDraw();

	static inline void ClearRenderTarget();

	static void Finalize();

	HRESULT GetHr() { return hr_; }

	void SetHr(HRESULT a) { this->hr_ = a; }

	ID3D12Device* GetDevice() { return device_; }

	ID3D12GraphicsCommandList* GetCommandList() { return commandList_; }

private:
	void InitializeDXGIDevice();

	void InitializeCommand();

	void CreateSwapChain();

	void CreateFinalRenderTargets();

	void CreateFence();

private:
	static WinApp* win_;

	//　DXGIファクトリー生成
	static IDXGIFactory7* dxgiFactory_;
	//

	// 使用するアダプタ用の変数
	static IDXGIAdapter4* useAdapter_;
	//

	// D3D12Device生成
	static ID3D12Device* device_;
	//

	// コマンドキューを生成する
	static ID3D12CommandQueue* commandQueue_;
	//

	// コマンドアロケータを生成する
	static ID3D12CommandAllocator* commandAllocator_;
	//

	// コマンドリストの生成
	static ID3D12GraphicsCommandList* commandList_;
	//

	// スワップチェーンを生成する
	static IDXGISwapChain4* swapChain_;
	//

	// ディスクリプタヒープの生成
	static ID3D12DescriptorHeap* rtvDescriptorHeap_;
	//

	// SwapchainからResourceを引っ張ってくる
	static ID3D12Resource* swapChainResources_[2];
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	//

	// 初期値0でFenceを作る
	static ID3D12Fence* fence_;
	static uint64_t fenceValue_;
	//

	// FenceのSignalを持つためにイベントを作成する
	static HANDLE fenceEvent_;

	static int32_t backBufferWidth_;
	static int32_t backBufferHeight_;

	static inline D3D12_RESOURCE_BARRIER barrier_{};

	static HRESULT hr_;
	//
};