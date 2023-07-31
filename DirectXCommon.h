#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <format>
#include <dxgidebug.h>
#include "ConvertString.h"
#include "WinApp.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")

class DirectXCommon
{
public:

	DirectXCommon();

	~DirectXCommon();

	void Initialize(HWND hwnd);

	void PreDraw();

	void PostDraw();

	HRESULT GetHr() { return hr_; }

	void SetHr(HRESULT a) { this->hr_ = a; }

	ID3D12Device* GetDevice()const { return device_; }

	ID3D12GraphicsCommandList* GetCommandList()const { return commandList_; }

	IDXGISwapChain4* GetSwapChain()const { return swapChain_; };

	ID3D12Resource* GetSwapChainResource(int index)const { return swapChainResources_[index]; };

	D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle(int index)const{
		assert(index >= 0 && index < 2);
		return rtvHandles_[index];
	}

	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue_; };

	ID3D12CommandAllocator* GetCommandAllocator()const { return commandAllocator_; };

	ID3D12Fence* GetFence()const { return fence_; };

	uint64_t GetFenceValue()const { return fenceValue_; };

	HANDLE GetFenceEvent() { return fenceEvent_; };

	static void ImGuiInitialize();

	ID3D12DescriptorHeap* GetsrvDescriptor() { return srvDescriptorHeap_; }

private:
	void InitializeDXGIDevice();

	void InitializeCommand();

	void CreateSwapChain();

	void CreateFence();

	void PullResourceSwapChain();

	void CreateRTV();

	void Relese();

	void CommandKick();

private:
	static WinApp winApp_;

	static HWND hwnd_;

	//　DXGIファクトリー生成
	IDXGIFactory7* dxgiFactory = nullptr;
	//

	// 使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter = nullptr;
	//


	// ディスクリプタヒープの生成
	static ID3D12DescriptorHeap* rtvDescriptorHeap_;
	//

	static D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;


	static ID3D12DescriptorHeap* srvDescriptorHeap_;


	static HRESULT hr_;

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
	static DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
	//

	// SwapchainからResourceを引っ張ってくる
	static ID3D12Resource* swapChainResources_[2];
	static D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	//

	// 初期値0でFenceを作る
	static ID3D12Fence* fence_;
	uint64_t fenceValue_;
	//

	// FenceのSignalを持つためにイベントを作成する
	HANDLE fenceEvent_;

	//

	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

};