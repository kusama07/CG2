#include "WinApp.h"

WinApp::WinApp() {

	Title_ = L"CG2_LE2B_10_クサマリョウト";

	hInst_ = nullptr;
	hwnd_ = nullptr;

	Width_ = 1280;
	Height_ = 720;

	wrc_ = {};
	wc_ = {};

}

WinApp::~WinApp() {
	End();
}

void WinApp::StartApp() {
	Initialize();
}

void WinApp::EndApp() {
	End();
}

//ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WinApp::Initialize() {
	if (!WindowClassRegister()) {
		return false;
	}
	return true;
}

bool WinApp::WindowClassRegister() {
	//ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;
	//クラス名
	wc_.lpszClassName = L"CG2WindowClass";
	//インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラス登録
	RegisterClass(&wc_);

	//ウィンドウサイズの構造体にクライアント領域を入れる
	wrc_ = { 0,0,Width_,Height_ };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,//クラス名
		Title_,//タイトルバーの名前
		WS_OVERLAPPEDWINDOW,//ウィンドウスタイル
		CW_USEDEFAULT,//表示X座標
		CW_USEDEFAULT,//表示Y座標
		wrc_.right - wrc_.left,//ウィンドウ横幅
		wrc_.bottom - wrc_.top,//ウィンドウ縦幅
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		wc_.hInstance,//インスタンスハンドル
		nullptr//オプション
	);

#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_))))
	{
		debugController_->EnableDebugLayer();
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	if (hwnd_ == nullptr) {
		return false;
	}

	//ウィンドウ表示
	ShowWindow(hwnd_, SW_SHOW);

	return true;

}

void WinApp::End() {
	EndWindow();
}

void WinApp::EndWindow() {

#ifdef _DEBUG
	if (debugController_ != nullptr) {
		debugController_->Release();
	}

#endif
}
