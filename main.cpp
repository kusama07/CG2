#include "WindowsClass.h"
#include "DirectXClass.h"

const wchar_t kWindowTitle[] = { L"CG2" };

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WindowsClass* win = nullptr;

	// ゲームウィンドウの作成
	WindowsClass::GetWindow(kWindowTitle);

	DirectXClass::DirectXInitialization();
	
	MSG msg{};

	//ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//ゲームの処理
			
		}
	}
}