#pragma once
#include <cstdint>
#include <Windows.h>

class WindowsClass
{
public:
	//ウィンドウサイズ
	static const int kClientWidth = 1280;
	static const int kClientHeight = 720;


	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND GetHwnd() const { return hwnd_; }

	static void GetWindow(
		const wchar_t* title = L"CG2");

private:
	// ウィンドウクラス
	static inline WNDCLASS wc{};

	//ウィンドウオブジェクト
	static HWND hwnd_;

};

