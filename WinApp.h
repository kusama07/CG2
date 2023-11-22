#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WinApp
{
public:
	WinApp();
	~WinApp();

	void StartApp();
	void EndApp();
	
	int32_t GetWidth() const { return Width_; }

	int32_t GetHeight() const { return Height_; }

	HWND GetHwnd() const { return hwnd_; }

	HINSTANCE GetHInst() const { return hInst_; }

	// メッセージの処理
	bool ProcessMessage();

private:
	const wchar_t* Title_;
	HINSTANCE hInst_;
	HWND hwnd_;
	int32_t Width_;		
	int32_t Height_;

	RECT wrc_;

	WNDCLASS wc_;

	ID3D12Debug1* debugController_ = nullptr;

	bool Initialize();

	void End();

	bool WindowClassRegister();

	void EndWindow();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};