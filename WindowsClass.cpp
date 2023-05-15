#include "windowsClass.h"

WindowsClass* WindowsClass::GetInstance() {
	static WindowsClass instance;
	return &instance;
}

LRESULT WindowsClass::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲームの固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}

void WindowsClass::GetWindow(const wchar_t* title , UINT windowStyle,
	int32_t clientWidth , int32_t clientHeight) {
	OutputDebugStringA("Hello,DirectX!!\n");

	//ウィンドウクラスの設定
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc; //ウィンドウプロシージャを設定
	wc.lpszClassName = L"CG2";         //ウィンドウクラス名
	wc.hInstance = GetModuleHandle(nullptr);   //ウィンドウハンドル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);  //カーソル指定

	//ウィンドウクラスをOSに登録する
	RegisterClass(&wc);
	//ウィンドウサイズ｛ X座標 Y座標 横幅 縦幅 ｝
	RECT wrc = { 0,0,clientWidth,clientHeight };
	//自動でサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,//クラス名
		L"CG2",       //タイトルバーの文字
		WS_OVERLAPPEDWINDOW,  //標準的なウィンドウスタイル
		CW_USEDEFAULT,        //表示X座標 (OSに任せる)
		CW_USEDEFAULT,        //表示Y座標 (OSに任せる)
		wrc.right - wrc.left, //ウィンドウ横幅
		wrc.bottom - wrc.top, //ウィンドウ縦幅
		nullptr,              //親ウィンドウハンドル
		nullptr,              //メニューハンドル
		wc.hInstance,         //呼び出しアプリケーションハンドル
		nullptr);	          //オプション

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG

	ShowWindow(hwnd, SW_SHOW);
}

