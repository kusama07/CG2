#pragma once
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include "WinApp.h"
#include <cassert>
#include <guiddef.h>
#include <wrl.h>
#include <windows.h>

class Input
{
public:
	// namespace省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public: 
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	void Updata();

	static Input* GetInstance();

	bool NoneKey(BYTE keyNumber);
	bool ReleassKey(BYTE keyNumber);

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);

private:
	WinApp* winApp_ = nullptr;

	WNDCLASS w_;
	HRESULT result_;

	ComPtr<IDirectInput8> directInput_ = nullptr;
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

	BYTE keys_[256] = {};
	BYTE preKeys_[256] = {};
};

