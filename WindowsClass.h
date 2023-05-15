#pragma once
#include <cstdint>
#include <Windows.h>

class WindowsClass
{
public:
	//ウィンドウサイズ
	static const int kClientWidth = 1280;
	static const int kClientHeight = 720;

	void GetWindow(
		const wchar_t* title = L"CG2", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWidth = kClientWidth, int32_t clientHeight = kClientHeight);

	static WindowsClass* GetInstance();

	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	HWND hwnd = nullptr;

};

