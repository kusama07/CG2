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
	static DirectX* GetInstance();


private:
	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;

	ID3D12Device* device = nullptr;

	void InitializeDXGIDevice();


};
