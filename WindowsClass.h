#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class WindowsClass
{
public:
	//クライアント領域サイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	HINSTANCE GetHInstance()const { return wc_.hInstance; }

	static	bool Procesmessage();
	static void Finalize();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static ID3D12Debug1* GetdebugController() { return debugController_; }

	static inline HWND GetHwnd() { return hwnd_; }

	static void CreateWindowView(const wchar_t* title, int32_t clientWidth, int32_t clientheight);

private:
	static	UINT windowStyle_;

	static ID3D12Debug1* debugController_;

	static	inline 	RECT wrc_ = { 0,0,kClientWidth,kClientHeight };

	static inline	WNDCLASS wc_{};// ウィンドウクラス

	static	HWND hwnd_;
};