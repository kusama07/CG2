#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <format>
#include "WindowsClass.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DirectX {
public:

	void Initialize(
		WindowsClass* win);

	static DirectX* GetInstance();

private:

	WindowsClass* win_;

	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;

	ID3D12Device* device = nullptr;

	// コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;

	// コマンドアプリケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;

	// コマンドリストの生成
	ID3D12GraphicsCommandList* commandList = nullptr;

	IDXGISwapChain4* swapChain = nullptr;

	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;

	void InitializeDXGIDevice();

	void CreateSwapChain();

	void InitializeCommand();

	void CreateFinalRenderTargets();

};
